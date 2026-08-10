#if defined(ESP32) || defined(ESP_PLATFORM)

#include "heater/MenuBridge.h"

#include <string.h>

#include <hal/hal_types.h>
#include <local_access/device_publisher.h>

// Сгенерированные артефакты меню (v3_nvs).
#include <menu_bindings.h>
#include <menu_cache.h>
#include <menu_commands.h>
#include <menu_ids.h>
#include <menu_meta.h>
#include <menu_nvs_io.h>
#include <menu_state.h>
#include <menu_types.h>

namespace iheaterlink {

namespace {

/// Три toggle в меню ПОДКЛЮЧЕНИЯ, из которых одновременно может быть активен
/// только один. Порядок важен только для лога.
constexpr const char *kConnectionToggles[] = {"bambu_en", "moon_en", "ha_en"};
constexpr size_t kConnectionTogglesCount =
    sizeof(kConnectionToggles) / sizeof(kConnectionToggles[0]);

/// Выключить все toggle ПОДКЛЮЧЕНИЙ кроме `keepBind`.
/// Возвращает количество отключённых.
int disableOtherConnections(const char *keepBind) {
  int off = 0;
  for (size_t i = 0; i < kConnectionTogglesCount; i++) {
    const char *b = kConnectionToggles[i];
    if (keepBind && strcmp(b, keepBind) == 0)
      continue;
    bool cur = false;
    if (menu_read_by_bind(b, &cur) && cur) {
      menu_apply_by_bind(b, 0.0f);
      off++;
      HAL_LOG_INFO("MENU", "exclusivity: %s → OFF", b);
    }
  }
  return off;
}

/// Если после загрузки NVS оказалось включено больше одного — оставить первый,
/// остальные сбросить. Invariant: максимум один активный.
void normalizeExclusivity() {
  const char *firstOn = nullptr;
  for (size_t i = 0; i < kConnectionTogglesCount; i++) {
    bool cur = false;
    if (menu_read_by_bind(kConnectionToggles[i], &cur) && cur) {
      if (!firstOn)
        firstOn = kConnectionToggles[i];
      else {
        menu_apply_by_bind(kConnectionToggles[i], 0.0f);
        HAL_LOG_WARN("MENU", "normalize: duplicate active toggle %s → OFF",
                     kConnectionToggles[i]);
      }
    }
  }
}

} // namespace

void MenuBridge::begin() {
  if (nvsReady_)
    return;

  // 1. Открыть NVS namespace (записывается один раз, держим открытым).
  if (!menu_nvs_begin()) {
    HAL_LOG_WARN("MENU",
                 "NVS begin failed — работаем только с дефолтами в RAM");
  }

  // 2. Выставить дефолты из YAML (перекроет предыдущее состояние MenuState).
  menu.initDefaults();

  // 3. Записывает magic/version в NVS если отсутствуют — без них loadFromNVS() пропустит чтение.
  {
    uint32_t magic = 0, ver = 0;
    ee_read(NVS_KEY_MAGIC, magic);
    ee_read(NVS_KEY_VERSION, ver);
    if (magic != NVS_MENU_MAGIC || ver != (uint32_t)NVS_MENU_VERSION) {
      ee_write(NVS_KEY_MAGIC, (uint32_t)NVS_MENU_MAGIC);
      ee_write(NVS_KEY_VERSION, (uint32_t)NVS_MENU_VERSION);
      HAL_LOG_INFO("MENU", "NVS header initialized (magic=0x%08X ver=%u)",
                   (unsigned)NVS_MENU_MAGIC, (unsigned)NVS_MENU_VERSION);
    }
  }

  // 4. Подтянуть сохранённые значения (magic/version проверяются внутри).
  menu.loadFromNVS();

  // 5. Нормализовать эксклюзивность ПОДКЛЮЧЕНИЙ (если в NVS два+ активных).
  normalizeExclusivity();

  // 6. Синхронизировать MenuState → g_menu_cache (для menu_buildFullJson).
  //    Generic функция из autogen — генератор внутри menu_apply_by_bind тоже
  //    sync'ит на каждый set, так что после bootstrap кэш всегда актуален.
  menu_sync_state_to_cache();

  // 6a. Pre-allocate MenuPublisher (один malloc на heap MENU_SERIALIZED_MAX_SIZE
  //     + DynamicJsonDocument). Без этого publishFullConfig() сразу упадёт.
  //     Если init не удался — логируем и продолжаем; publishFullConfig вернёт
  //     false до тех пор пока memory не освободится.
  if (!menuPub_.begin()) {
    HAL_LOG_ERROR("MENU",
                  "MenuPublisher init failed (heap exhausted?) — "
                  "publishFullConfig will fail");
  }

  nvsReady_ = true;

  // 7. Сразу эмитим текущий active чтобы LinkIntegrationsManager применил
  //    выбранный источник (важно если в NVS уже было включено, скажем,
  //    moon_en).
  lastActive_ = ActiveConnection::None; // заставим emit-if-changed сработать
  emitActiveIfChanged();

  // 8. Стартовый emit для ignore_external_cmd — продукт получит callback и
  //    вызовет link.setIgnoreExternalCmd(v) с актуальным значением из NVS.
  emitIgnoreExtCmdIfChanged();

  HAL_LOG_INFO("MENU", "Initialized (NVS namespace=%s, %u bindings)",
               NVS_MENU_NAMESPACE, (unsigned)g_bindings_count);
}

ActiveConnection MenuBridge::currentActive() const {
  bool en = false;
  if (menu_read_by_bind("bambu_en", &en) && en)
    return ActiveConnection::Bambu;
  if (menu_read_by_bind("moon_en", &en) && en)
    return ActiveConnection::Moonraker;
  if (menu_read_by_bind("ha_en", &en) && en)
    return ActiveConnection::Ha;
  return ActiveConnection::None;
}

void MenuBridge::emitActiveIfChanged() {
  ActiveConnection cur = currentActive();
  if (cur == lastActive_)
    return;
  lastActive_ = cur;
  if (activeCb_)
    activeCb_(cur);
}

void MenuBridge::emitIgnoreExtCmdIfChanged() {
  bool cur = false;
  if (!menu_read_by_bind("ign_ext_cmd", &cur)) {
    return;
  }
  if (ignoreExtCmdInitialized_ && cur == lastIgnoreExtCmd_)
    return;
  lastIgnoreExtCmd_ = cur;
  ignoreExtCmdInitialized_ = true;
  if (ignoreExtCmdCb_)
    ignoreExtCmdCb_(cur);
}

bool MenuBridge::publishFullConfig() {
  if (!pub_)
    return false;
  if (!nvsReady_)
    begin();

  // MenuPublisher переиспользует pre-allocated heap-буфер и DynamicJsonDocument
  // (выделены один раз в begin()). Никаких malloc/free в горячем пути, нет
  // static char[] в .bss. DevicePublisher::publishConfigRaw делает dual-publish:
  // config уходит и в MQTT, и в локальный WS — LAN-клиент получает меню.
  size_t len = menuPub_.publishFull(pub_);
  if (len == 0) {
    HAL_LOG_ERROR("MENU", "menuPub_.publishFull returned 0 (overflow/init?)");
    return false;
  }
  HAL_LOG_INFO("MENU", "Published config: %u bytes", (unsigned)len);
  return true;
}

bool MenuBridge::applySetCommand(JsonObjectConst data) {
  if (!nvsReady_)
    begin();

  // Два способа адресации: по id (как у iDryer) или по bind (удобнее для
  // отладки).
  const MenuBinding *b = nullptr;

  if (data["bind"].is<const char *>()) {
    b = menu_find_bind(data["bind"].as<const char *>());
  } else if (data["id"].is<int>()) {
    int id = data["id"].as<int>();
    for (uint16_t i = 0; i < g_bindings_count; i++) {
      if (g_bindings[i].id == (uint16_t)id) {
        b = &g_bindings[i];
        break;
      }
    }
  }

  if (!b) {
    HAL_LOG_WARN("MENU", "set: не нашёл биндинг (ожидаем id или bind)");
    return false;
  }

  // Значение берём как float — menu_apply_by_bind сам приведёт к нужному vtype.
  float val = 0.0f;
  if (data["val"].is<float>())
    val = data["val"].as<float>();
  else if (data["val"].is<bool>())
    val = data["val"].as<bool>() ? 1.0f : 0.0f;
  else if (data["val"].is<int>())
    val = (float)data["val"].as<int>();
  else {
    HAL_LOG_WARN("MENU", "set: val не число/bool");
    return false;
  }

  // menu_apply_by_bind: store_value(MenuState) + NVS persist + on_change hook.
  bool ok = menu_apply_by_bind(b->bind, val);
  if (!ok) {
    HAL_LOG_WARN("MENU", "apply failed: bind=%s", b->bind);
    return false;
  }

  // Эксклюзивность ПОДКЛЮЧЕНИЙ: если включили bambu_en/moon_en/ha_en —
  // остальные два выключаем.
  if (val > 0.0f) {
    for (size_t i = 0; i < kConnectionTogglesCount; i++) {
      if (strcmp(b->bind, kConnectionToggles[i]) == 0) {
        disableOtherConnections(b->bind);
        break;
      }
    }
  }

  // Cache уже синхронизирован: menu_apply_by_bind() (autogen) делает sync
  // конкретного binding'а в g_menu_cache. Дополнительный sync не нужен.

  // После эксклюзивности сообщаем наверх о новом активном ПОДКЛЮЧЕНИИ.
  emitActiveIfChanged();

  // Если поменяли toggle игнора внешних команд — сообщаем продукту, чтобы
  // он вызвал link.setIgnoreExternalCmd(v) (SDK включит/выключит guard).
  if (strcmp(b->bind, "ign_ext_cmd") == 0) {
    emitIgnoreExtCmdIfChanged();
  }

  publishFullConfig();

  HAL_LOG_INFO("MENU", "set: %s = %.3f (id=%u)", b->bind, (double)val,
               (unsigned)b->id);
  return true;
}

bool MenuBridge::applyInvokeCommand(JsonObjectConst data) {
  if (!nvsReady_)
    begin();

  // Адресация только по id (action не имеет bind).
  if (!data["id"].is<int>()) {
    HAL_LOG_WARN("MENU", "invoke: ожидается поле id");
    return false;
  }
  uint16_t id = (uint16_t)data["id"].as<int>();

  if (id >= MENU__COUNT) {
    HAL_LOG_WARN("MENU", "invoke: id=%u вне диапазона", (unsigned)id);
    return false;
  }

  const MenuItem &item = g_menu[id];
  if (item.type != MN_ACTION) {
    HAL_LOG_WARN("MENU", "invoke: id=%u не action (type=%d)", (unsigned)id,
                 (int)item.type);
    return false;
  }
  if (!item.u.action.invoke) {
    HAL_LOG_WARN("MENU", "invoke: id=%u нет on_invoke", (unsigned)id);
    return false;
  }

  // args — для будущих сложных команд (profile/rfid_write/led_pulse).
  // Сейчас action функции void(void) — args игнорируются.
  HAL_LOG_INFO("MENU", "invoke: id=%u", (unsigned)id);
  item.u.action.invoke();
  return true;
}

} // namespace iheaterlink

#endif // ESP32
