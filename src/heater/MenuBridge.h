/**
 * @file MenuBridge.h
 * @brief Мост между меню (NVS-backed MenuState + g_menu_cache) и MQTT-порталом.
 *
 * Задачи:
 *   - Инициализация NVS namespace меню, загрузка сохранённых значений, синхронизация
 *     с `g_menu_cache` (который использует `menu_buildFullJson()` для отдачи config).
 *   - Публикация полного меню (`{v, menu:[...]}`) в MQTT топик `config`
 *     в ответ на `commands/get_config`.
 *   - Применение одиночных правок (`commands/set` → `menu_apply_by_bind` → NVS persist),
 *     обновление `g_menu_cache` и повторная публикация полного config.
 */

#pragma once

#include <ArduinoJson.h>
#include <functional>

#include <menu_publisher.h>  // idryer::MenuPublisher для pre-allocated публикации меню

// Forward declaration — чтобы не тянуть весь device_publisher.h в публичный
// заголовок. DevicePublisher шлёт config в MQTT И в локальный WS (dual-publish),
// чтобы LAN-клиент тоже получал меню в ответ на get_config.
namespace idryer { class DevicePublisher; }

namespace iheaterlink {

/// Идентификатор выбранного в меню «ПОДКЛЮЧЕНИЯ» источника target температуры.
/// Соответствует значению toggle-поля (bambu_en / moon_en / ha_en).
enum class ActiveConnection : uint8_t { None = 0, Bambu, Moonraker, Ha };

/// Колбэк на смену активного ПОДКЛЮЧЕНИЯ — вызывается из MenuBridge при
/// изменении toggle-ов или при старте (синхронизация с сохранённым NVS).
using ActiveConnectionCallback = std::function<void(ActiveConnection)>;

/// Колбэк на изменение toggle "Игнор. внеш. команд". Вызывается из begin()
/// со стартовым значением (из NVS) и далее при каждом applySetCommand,
/// которое поменяло этот bind.
using IgnoreExternalCmdCallback = std::function<void(bool)>;

class MenuBridge {
public:
    explicit MenuBridge(idryer::DevicePublisher* pub) : pub_(pub) {}

    /// Зарегистрировать колбэк на смену активного ПОДКЛЮЧЕНИЯ.
    /// Будет вызван один раз из begin() со стартовым значением и далее
    /// при каждом applySetCommand, которое поменяло bambu_en/moon_en/ha_en.
    void setActiveConnectionCallback(ActiveConnectionCallback cb) { activeCb_ = std::move(cb); }

    /// Зарегистрировать колбэк на изменение toggle "Игнор. внеш. команд".
    /// Будет вызван один раз из begin() со стартовым значением (из NVS) и
    /// далее при каждом applySetCommand, которое поменяло этот bind.
    void setIgnoreExternalCmdCallback(IgnoreExternalCmdCallback cb) { ignoreExtCmdCb_ = std::move(cb); }

    /// Открыть NVS, подтянуть дефолты, загрузить сохранённые значения,
    /// синхронизировать MenuState → g_menu_cache.
    /// Безопасно вызывать повторно (no-op после первого успеха).
    void begin();

    /// Построить полный JSON меню и опубликовать в MQTT `config`.
    /// Возвращает true если публикация успешна.
    bool publishFullConfig();

    /// Применить одиночное изменение значения.
    /// Ожидаемый JSON: {"id":<int>, "val":<num|bool>} или {"bind":"<name>", "val":<num|bool>}.
    /// Обновляет MenuState, g_menu_cache и NVS; публикует обновлённый config.
    /// @return true если правка применена.
    bool applySetCommand(JsonObjectConst data);

    /// @brief commands/invoke по контракту menu_protocol_v1.
    /// JSON: {"id":<int>, "args"?:{...}, "timestamp"?: "..."}.
    /// Находит menu item по id, если type=action — вызывает on_invoke.
    /// args сейчас игнорируются (action функции бесаргументные); сложные
    /// команды с args (profile/rfid_write/led_pulse) — отдельная задача
    /// согласно ___capabilities_and_menu_as_protocol.md §6.5/§10.2.
    /// @return true если action найден и вызван.
    bool applyInvokeCommand(JsonObjectConst data);

private:
    /// Прочитать текущее состояние bambu_en/moon_en/ha_en и вернуть
    /// активное подключение (или None если все выключены).
    ActiveConnection currentActive() const;

    /// Вызвать activeCb_ если значение изменилось с прошлого вызова.
    void emitActiveIfChanged();

    /// Вызвать ignoreExtCmdCb_ если значение изменилось с прошлого вызова.
    void emitIgnoreExtCmdIfChanged();

    idryer::DevicePublisher* pub_ = nullptr;
    bool nvsReady_ = false;
    ActiveConnectionCallback activeCb_;
    ActiveConnection lastActive_ = ActiveConnection::None;
    IgnoreExternalCmdCallback ignoreExtCmdCb_;
    bool lastIgnoreExtCmd_ = false;
    bool ignoreExtCmdInitialized_ = false;
    /// Pre-allocated публикатор полного config — один malloc на старте,
    /// переиспользуется на каждый publishFullConfig. Заменяет старую логику
    /// со static char buf[MENU_FULL_JSON_BUF_SIZE] в .bss.
    idryer::MenuPublisher menuPub_;
};

} // namespace iheaterlink
