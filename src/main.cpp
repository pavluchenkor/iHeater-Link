// iHeater Link — точка входа: RMT-выход → SDK begin() → MenuBridge →
// onCommand-обработчики.

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <iDryer.h>
#include <idryer_integrations.h>
#include <integrations/common/link_integrations_types.h>
#include <runtime/idryer_runtime.h>

#include <core/idryer_errbus.h>

#include "controller/RmtOutputAdapter.h"
#include "heater/MenuBridge.h"
#include "heater/auto_heat.h"

#include "version.h"       // VERSION_STR для Config.firmwareVersion
#include <ota_receiver.h>  // idryer::OtaReceiver для Phase 6 OTA
#include <hal/hal_types.h> // g_hal->logger->setLevel()
#include <menu_bindings.h> // menu_apply_by_bind
#include <menu_meta.h>     // g_menu_meta[].min_val / max_val для HA-number
#include <menu_nvs_io.h>   // menu_nvs_begin
#include <menu_state.h> // menu.log_portal / log_printer / log_device / log_debug

// ── Конфигурация устройства ──────────────────────────────────────────────────
// Заполняется один раз при портировании прошивки на новый продукт.
// После изменения — перепрошить устройство; портал подхватит при следующем
// boot.
static const iDryer::Config CFG = {
    // Тип устройства — определяет логику карточки на портале.
    // Варианты: Dryer, Heater, StorageLink, IHeaterLink.
    .deviceType = iDryer::DeviceType::IHeaterLink,

    // Количество независимых камер с нагревателем (1 для iHeater Link).
    .unitsCount = 1,

    // ── Периферия ────────────────────────────────────────────────────────────
    // iHeater Link не имеет своих сенсоров — камеру меряет принтер.
    // hasHeater=true: в telemetry шлём 0/1 по текущей команде RMT.
    .hasHeater = true,       // управляемый нагреватель (мощность 0..1)
    .hasFan = false,         // вентилятор
    .hasLed = false,         // адресная LED-лента
    .hasWeight = false,      // весовой датчик
    .hasRfid = false,        // RFID-метка катушки
    .hasAirTemp = false,     // датчик температуры воздуха
    .hasAirHumidity = false, // датчик влажности воздуха
    .hasHeaterTemp = false,  // датчик температуры нагревателя

    // ── Интеграции ───────────────────────────────────────────────────────────
    .allowHa = true,        // Home Assistant MQTT
    .allowBambu = true,     // Bambu Lab LAN MQTT
    .allowMoonraker = true, // Moonraker WebSocket

    // ── Периоды публикации ───────────────────────────────────────────────────
    .telemetryPeriodMs = 5000,
    .statusPeriodMs = 5000,

    // ── Идентификация (отображается на портале) ──────────────────────────────
    .hardwareVersion = "LINK-v1",
    .firmwareVersion = VERSION_STR,

    // Название продукта — отображается в колонке «Тип» на странице устройств.
    // Задаётся свободно: любая строка UTF-8.
    .model = "iHeater Link",
};

// Флаг portal-логирования: управляется через меню → bind log_portal.
static bool s_logPortal = false;

// RMT-выход на STM32 iHeater; шлёт кадр keepalive каждые framePeriodMs
// независимо от WiFi.
static iheaterlink::RmtOutputAdapter s_output{iheaterlink::RmtOutputConfig{}};

// Дедлайн авто-Off (абсолютный millis). 0 = таймер не активен.
// NB для аудита: время сессии намеренно считается двумя независимыми путями —
// здесь millis (точный, для авто-Off) и status.elapsedS[] (тик 1 Гц, для UI).
// Это НЕ дубль-баг: расхождение < 1 c на часах нагрева, чинить не нужно.
static uint32_t s_dryingDeadlineMs = 0;

// Указатель на MenuBridge — инициализируется в setup() после begin().
static iheaterlink::MenuBridge *s_menuBridge = nullptr;

// Счётчик для ротации тестовых ошибок (0..3).
static uint8_t s_errCycle = 0;

// Единственный экземпляр Link: создаётся при первом вызове (в setup)
static iDryer::Link &device() {
  static iDryer::Link instance(CFG);
  return instance;
}

// Применяет нагрев: RMT-команда + обновление status + publishStatusNow().
// duration=0 → без таймера.
static void applyHeating(uint8_t u, float targetTempC, uint32_t durationS,
                         iDryer::UnitMode mode, const char *tag) {
  iheaterlink::ControllerOutputCommand cmd;
  cmd.mode = iheaterlink::ControllerOutputMode::TargetTemperature;
  cmd.targetTempC = targetTempC;
  s_output.apply(cmd);
  s_dryingDeadlineMs = durationS ? (millis() + durationS * 1000u) : 0;
  device().status.mode[u] = mode;
  device().status.targetTempC[u] = targetTempC;
  device().status.durationS[u] = durationS;
  device().status.elapsedS[u] = 0;
  device().telemetry.heaterPower01[u] = 1.0f;
  Serial.printf("[CMD] %s temp=%.1f duration=%us\n", tag, (double)targetTempC,
                durationS);
  device().publishStatusNow();
}

static void applyStop(uint8_t u) {
  iheaterlink::ControllerOutputCommand cmd;
  cmd.mode = iheaterlink::ControllerOutputMode::Off;
  cmd.targetTempC = 0.0f;
  s_output.apply(cmd);
  s_dryingDeadlineMs = 0;
  device().status.mode[u] = iDryer::UnitMode::Idle;
  device().status.targetTempC[u] = 0.0f;
  device().status.durationS[u] = 0;
  device().status.elapsedS[u] = 0;
  device().telemetry.heaterPower01[u] = 0.0f;
  Serial.println("[CMD] Stop");
  device().publishStatusNow();
}

// menu_protocol_v1: action-callback'и для invoke {id} из инспектора меню.
// Перекрывают weak-stub'ы в menu_callbacks_weak.cpp. Читают параметры из
// меню (persist в NVS), позволяя пользователю менять их с того же экрана.
// heat_duration в минутах; 0 = греть бесконечно (без таймера auto-off).
extern "C" void heat_start(void) {
  applyHeating(0, menu.heat_temp, (uint32_t)menu.heat_duration * 60u,
               iDryer::UnitMode::Heating, "menu:heat_start");
}

extern "C" void heat_stop(void) { applyStop(0); }

// Последний опубликованный Bambu progress — для триггера publishStatusNow
// при значимом изменении (≥1%). 0xFF = «ещё не публиковали».
static uint8_t s_lastPublishedBambuProgress = 0xFF;

// Добавляет units[0].progressPercent из активной интеграции (Bambu) в status.
// Hook вызывается из Link::publishStatusNow перед отправкой payload.
static void enrichStatus(JsonObject root) {
  using AI = idryer::cloud::ActiveIntegration;
  auto *mgr = device().integrationsManager();
  if (!mgr || mgr->getActive() != AI::Bambu)
    return;
  if (device().status.mode[0] != iDryer::UnitMode::Heating)
    return;
  JsonArray units = root["units"];
  if (units.isNull() || units.size() == 0)
    return;
  const auto &ps = mgr->bambuPrinterStatus();
  units[0]["progressPercent"] = ps.progressPercent;
  s_lastPublishedBambuProgress = ps.progressPercent;
}

// Добавляет deviceType/active/outputMode/targetTempC в каждый пакет телеметрии.
static void enrichTelemetry(JsonObject root) {
  const auto cmd = s_output.getLastCommand();
  const bool heating =
      (cmd.mode == iheaterlink::ControllerOutputMode::TargetTemperature);

  using AI = idryer::cloud::ActiveIntegration;
  const AI activeAI = device().integrationsManager()->getActive();

  root["deviceType"] = iDryer::deviceTypeToString(CFG.deviceType);
  root["active"] = idryer::cloud::activeIntegrationToString(activeAI);
  root["outputMode"] = heating ? 1 : 0;
  root["targetTempC"] = cmd.targetTempC;

  // chamberTemp от активной интеграции — публикуем только при реальных данных.
  // У Bambu/Moonraker клиентов поле инициализировано NAN и обновляется лишь
  // когда принтер прислал значение (или has_sensor=1 у Klipper). Если NAN —
  // поле в JSON не пишем (telemetry_null_policy: отсутствие = нет данных).
  auto* mgr = device().integrationsManager();
  if (mgr) {
    float chamberT = NAN;
    if (activeAI == AI::Bambu) {
      chamberT = mgr->bambuPrinterStatus().chamberTemp;
    } else if (activeAI == AI::Moonraker) {
      chamberT = mgr->moonrakerStatus().chamberTemperature;
    }
    if (!isnan(chamberT)) root["chamberTemp"] = chamberT;
  }
}

// Синхронизирует флаги логирования из меню → клиентов интеграций.
// Вызывается при старте (после MenuBridge::begin) и после каждого set-команды.
static void applyLogFlags() {
  s_logPortal = (bool)menu.log_portal;
  auto *mgr = device().integrationsManager();
  if (mgr)
    mgr->setLogPayloads((bool)menu.log_printer);
  iheaterlink::setLogDecisions((bool)menu.log_device);
  if (idryer::hal::g_hal && idryer::hal::g_hal->logger) {
    idryer::hal::g_hal->logger->setLevel(menu.log_debug
                                             ? idryer::hal::LogLevel::Debug
                                             : idryer::hal::LogLevel::Info);
  }
}

// ─────────────────────────────────────────────────────────────────────────────

void setup() {
  // WiFi.persistent(false) ПЕРВОЙ строкой — Arduino не пишет WiFi-config в NVS.
  WiFi.persistent(false);

  // SDK + Improv РАНО — handleSerial должен быть готов перехватить байты от
  // portal ДО того как остальной setup съест CPU. Тонкий setup = стабильный
  // Improv.
  device().onClaimPin([](const char *pin, uint32_t expires) {
    Serial.printf("CLAIM_PIN:%s:%lu\n", pin, expires);
    Serial.flush();
  });
  menu_nvs_begin(); // NVS namespace до device.begin (известный init contract)
  device().begin();

  // Не шлём device-timestamp в publish: портал хранит своё серверное время
  // приёма, device-timestamp избыточен (экономия трафика). См. MqttClient.
  device().mqttClient()->setAddTimestamp(false);

  // Phase 6 OTA: регистрируем приёмник прошивки СРАЗУ после link.begin().
  // Подписка на firmware_update_* через onCommand + setOtaChunkCallback.
  // markCurrentBootValid отменяет bootloader rollback — если эта прошивка
  // только что прилетела через OTA и впервые загрузилась, помечаем её как
  // валидную (иначе bootloader откатит при следующем ребуте).
  idryer::OtaReceiver::instance().begin(&device(), "iheater_link");
  idryer::OtaReceiver::markCurrentBootValid();

  // RMT и всё остальное — после.
  s_output.begin();

  // 3. MenuBridge: загружает NVS, эмитит активную интеграцию. Колбэк
  // назначается ДО begin().
  static iheaterlink::MenuBridge s_menuBridgeInst(device().mqttClient());
  s_menuBridge = &s_menuBridgeInst;

  auto *mgr = device().integrationsManager();
  s_menuBridgeInst.setActiveConnectionCallback(
      [mgr](iheaterlink::ActiveConnection kind) {
        using AI = idryer::cloud::ActiveIntegration;
        AI target = AI::None;
        switch (kind) {
        case iheaterlink::ActiveConnection::Bambu:
          target = AI::Bambu;
          break;
        case iheaterlink::ActiveConnection::Moonraker:
          target = AI::Moonraker;
          break;
        case iheaterlink::ActiveConnection::Ha:
          target = AI::Ha;
          break;
        default:
          break;
        }
        mgr->setActive(target);
      });

  // Связь меню «Игнор. внеш. команды» → SDK gate. Колбэк назначается ДО begin(),
  // чтобы стартовое значение из NVS было применено сразу.
  s_menuBridgeInst.setIgnoreExternalCmdCallback(
      [](bool enabled) { device().setIgnoreExternalCmd(enabled); });
  s_menuBridgeInst.begin(); // загружает NVS, эмитит активную интеграцию
  applyLogFlags(); // синхронизирует log_portal/log_printer/log_device из NVS

  // 4. Авто-нагрев: VirtualChamber (Moonraker) и BambuPrinterStatus → RMT.
  //    wireAutoHeat сохраняет указатель на s_output до подписки колбэков.
  iheaterlink::wireAutoHeat(&s_output);
  iheaterlink::wireBambuSession([](float targetTempC, bool heating) {
    if (heating) {
      applyHeating(0, targetTempC, 0, iDryer::UnitMode::Heating, "Bambu");
    } else {
      applyStop(0);
    }
  });
  iheaterlink::wireMoonrakerSession([](float targetTempC, bool heating) {
    if (heating) {
      applyHeating(0, targetTempC, 0, iDryer::UnitMode::Heating, "Klipper");
    } else {
      applyStop(0);
    }
  });
  mgr->setVirtualChamberCallback(iheaterlink::onVirtualChamberUpdate);
  mgr->setBambuPrinterStatusCallback(iheaterlink::onBambuPrinterStatusUpdate);

  // 5. Добавляем deviceType/active/outputMode/targetTempC в каждый publish
  // телеметрии.
  device().onTelemetryPublish(enrichTelemetry);
  // Аналогично — обогащаем status (units[0].progressPercent от Bambu).
  device().onStatusPublish(enrichStatus);

  // 6. Периодические задачи через cooperative scheduler фасада.
  //    Тик elapsedS раз в секунду, пока режим активен.
  device().every(1000, []() {
    const auto m = device().status.mode[0];
    if (m == iDryer::UnitMode::Heating || m == iDryer::UnitMode::Storage)
      device().status.elapsedS[0]++;
  });
  //    Триггер publishStatusNow при ∆Bambu progress ≥ 1% — иначе status
  //    шлётся только при смене mode/target и прогресс «застывает». 1 Гц
  //    совпадает с частотой push_status от Bambu, спама не будет.
  device().every(1000, []() {
    using AI = idryer::cloud::ActiveIntegration;
    auto *mgr = device().integrationsManager();
    if (!mgr || mgr->getActive() != AI::Bambu)
      return;
    if (device().status.mode[0] != iDryer::UnitMode::Heating)
      return;
    const uint8_t cur = mgr->bambuPrinterStatus().progressPercent;
    const uint8_t last = s_lastPublishedBambuProgress;
    const uint8_t diff = (cur > last) ? (cur - last) : (last - cur);
    if (last == 0xFF || diff >= 1)
      device().publishStatusNow();
  });
  //    Авто-Off по deadline drying — раз в полсекунды проверяем дедлайн.
  device().every(500, []() {
    if (!s_dryingDeadlineMs)
      return;
    if ((int32_t)(millis() - s_dryingDeadlineMs) < 0)
      return;
    iheaterlink::ControllerOutputCommand cmd;
    cmd.mode = iheaterlink::ControllerOutputMode::Off;
    cmd.targetTempC = 0.0f;
    s_output.apply(cmd);
    s_dryingDeadlineMs = 0;
    device().status.mode[0] = iDryer::UnitMode::Idle;
    device().status.targetTempC[0] = 0.0f;
    device().status.durationS[0] = 0;
    device().status.elapsedS[0] = 0;
    device().telemetry.heaterPower01[0] = 0.0f;
    device().publishStatusNow();
    Serial.println("[CMD] drying deadline expired → Off");
  });

  // 7. HA controls — продуктовые сущности в HA UI. Привязаны к menu-полям
  //    (heat_temp / heat_duration) через menu_apply_by_bind: значение
  //    persist'ится в NVS, синхронизируется с инспектором портала.
  //    Диапазоны (min/max) берутся из g_menu_meta — единый источник правды
  //    с menu.yaml, при правке YAML и регенерации HA автоматически подхватит.
  //    Кнопка Heat start читает текущие menu.heat_temp / menu.heat_duration
  //    (тот же путь что invoke по id из DeviceMenuPanel).
  auto &ha = device().ha();
  ha.number(
      "heat_temp", "Heat temperature",
      (int)g_menu_meta[MENU_HEAT_TEMP].min_val,
      (int)g_menu_meta[MENU_HEAT_TEMP].max_val,
      [](int v) { menu_apply_by_bind("heat_temp", (float)v); },
      "°C", "mdi:thermometer");
  ha.number(
      "heat_duration", "Heat duration",
      (int)g_menu_meta[MENU_HEAT_DURATION].min_val,
      (int)g_menu_meta[MENU_HEAT_DURATION].max_val,
      [](int v) { menu_apply_by_bind("heat_duration", (float)v); },
      "min", "mdi:timer-outline");
  ha.button("heat_start", "Heat start", []() { heat_start(); },
            "mdi:play-circle");
  ha.button("stop", "Stop", []() { applyStop(0); }, "mdi:stop-circle");

  // 8. Команды портала / Local-WS — обработчики через onCommand.
  //    iHeater Link на menu_protocol_v1: запуск/остановка нагрева — только
  //    через invoke (heat.start / heat.stop, форма B). Legacy
  //    commands/drying / commands/storage / commands/stop здесь больше не
  //    обрабатываются (backend для IHEATER_LINK шлёт invoke).
  auto &link = device();

  link.onCommand(
      "find", [](JsonObjectConst) { /* iHeater Link не имеет индикатора */ });
  link.onCommand("clear_errors", [](JsonObjectConst) { /* нет UART-ошибок */ });

  // get_config / set / invoke — команды управления меню (menu_protocol_v1).
  link.onCommand("get_config", [](JsonObjectConst) {
    if (s_menuBridge)
      s_menuBridge->publishFullConfig();
  });
  link.onCommand("set", [](JsonObjectConst data) {
    if (s_logPortal) {
      char buf[256];
      serializeJson(data, buf, sizeof(buf));
      HAL_LOG_INFO("PORTAL", "← set %s", buf);
    }
    if (s_menuBridge)
      s_menuBridge->applySetCommand(data);
    applyLogFlags();
  });
  // invoke: heat.start/heat.stop — здесь; остальные действия → MenuBridge.
  link.onCommand("invoke", [](JsonObjectConst data) {
    const char *action = data["action"] | "";
    JsonObjectConst args = data["args"];

    if (strcmp(action, "heat.start") == 0) {
      float tempC = args["tempC"] | 0.0f;
      uint32_t durMin = args["durationMin"] | 0u;
      applyHeating(0, tempC, durMin * 60u, iDryer::UnitMode::Heating,
                   "invoke:heat.start");
    } else if (strcmp(action, "heat.stop") == 0) {
      applyStop(0);
    } else if (s_menuBridge) {
      s_menuBridge->applyInvokeCommand(data);
    }
  });

  // link_integration: либа переключает интеграцию, здесь — синхронизируем
  // меню-тогглы.
  link.onCommand("link_integration", [](JsonObjectConst data) {
    if (s_logPortal) {
      char buf[256];
      serializeJson(data, buf, sizeof(buf));
      HAL_LOG_INFO("PORTAL", "← link_integration %s", buf);
    }
    const char *type = data["type"] | (const char *)nullptr;
    const bool enabled = data["enabled"] | false;
    if (!type || !enabled || !s_menuBridge)
      return;
    const char *bind = nullptr;
    if (strcmp(type, "moonraker") == 0)
      bind = "moon_en";
    else if (strcmp(type, "bambu") == 0)
      bind = "bambu_en";
    else if (strcmp(type, "ha") == 0)
      bind = "ha_en";
    if (!bind)
      return;
    StaticJsonDocument<32> doc;
    doc["bind"] = bind;
    doc["val"] = true;
    s_menuBridge->applySetCommand(doc.as<JsonObjectConst>());
  });

  // 9. Шина ошибок → raiseEvent() → MQTT events topic.
  //    Единый словарь severity с errbus: INFO/WARN/ERROR/CRIT.
  error_set_handler([](const ErrorEvent *ev) {
    iDryer::EventKind kind;
    switch (ev->severity) {
    case ERRSEV_INFO:
      kind = iDryer::EventKind::Info;
      break;
    case ERRSEV_WARNING:
      kind = iDryer::EventKind::Warning;
      break;
    case ERRSEV_CRITICAL:
      kind = iDryer::EventKind::Critical;
      break;
    default:
      kind = iDryer::EventKind::Error;
      break;
    }
    char event_key[48];
    snprintf(event_key, sizeof(event_key), "%s_%s", errsrc_name(ev->source),
             errcode_name(ev->code));
    device().raiseEvent(kind, event_key, ev->msg, ev->ctrl_id);
  });

  // 10. Тестовые ошибки — закомментировано, верификация пройдена.
  // device().every(20000, []() {
  //     static const ErrSeverity kSev[]  = { ERRSEV_INFO, ERRSEV_WARNING,
  //     ERRSEV_ERROR, ERRSEV_CRITICAL }; static const ErrSource   kSrc[]  = {
  //     ERRSRC_HEATER, ERRSRC_THERM, ERRSRC_AIR, ERRSRC_LINK }; static const
  //     ErrCode     kCode[] = { ERRC_OVER_MAX, ERRC_SENSOR_INVALID,
  //     ERRC_TIMEOUT, ERRC_NO_RESPONSE }; static const char*       kMsg[]  = {
  //         "test: heater over max", "test: sensor invalid",
  //         "test: air timeout",     "test: link no response"
  //     };
  //     const uint8_t i = s_errCycle++ & 3;
  //     POST_ERROR(kSev[i], 0, kSrc[i], kCode[i], kMsg[i],
  //     static_cast<int32_t>(i * 100));
  // });
}

void loop() {
  device()
      .loop(); // WiFi/MQTT/LocalAccess + auto-telemetry/status + every() tasks
  error_process_all(); // шина ошибок → error_set_handler → raiseEvent
}
