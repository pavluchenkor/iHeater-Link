#if defined(ESP32) || defined(ESP_PLATFORM)

#include "heater/auto_heat.h"
#include "heater/material_temp.h"
#include "controller/RmtOutputAdapter.h"
#include <string.h>

#include <hal/hal_types.h>
#include <integrations/common/link_integrations_types.h>
#include <integrations/moonraker/moonraker_client.h>   // VirtualChamberData
#include <integrations/bambu/bambu_client.h>           // BambuPrinterStatus

#include <menu_state.h>

namespace iheaterlink {

namespace {
RmtOutputAdapter* g_output           = nullptr;
bool              g_logDecisions     = false;
SessionCallback   g_bambuSession     = nullptr;
SessionCallback   g_moonrakerSession = nullptr;
// Дедуп: вызываем session-колбэк только при смене heating/target. Иначе на
// каждом push_status (Bambu ~1 Hz) / Klipper update триггерили бы
// publishStatusNow и засоряли MQTT портала. initialized=true сразу — чтобы
// первый Off на старте не дёргал лишний applyStop.
struct SessionPrev {
    bool  heating = false;
    float target  = 0.0f;
};
SessionPrev g_bambuPrev;
SessionPrev g_moonrakerPrev;
}

void wireAutoHeat(RmtOutputAdapter* output) {
    g_output = output;
}

void wireBambuSession(SessionCallback cb) {
    g_bambuSession = cb;
}

void wireMoonrakerSession(SessionCallback cb) {
    g_moonrakerSession = cb;
}

void setLogDecisions(bool enabled) {
    g_logDecisions = enabled;
}

// VirtualChamber из Moonraker. Колбэк приходит только от активной интеграции:
// неактивный клиент выключен (LinkIntegrationsManager::applyIntegrations), так
// что отдельного тумблера-гейта не нужно — источник правды один.
// Поведение: available → target>0 → ON, иначе OFF.
void onVirtualChamberUpdate(void* /*ctx*/, const idryer::cloud::VirtualChamberData& data) {
    if (!g_output) return;

    ControllerOutputCommand cmd{};
    if (!data.available || data.target <= 0.0f) {
        cmd.mode = ControllerOutputMode::Off;
        cmd.targetTempC = 0.0f;
    } else {
        cmd.mode = ControllerOutputMode::TargetTemperature;
        cmd.targetTempC = data.target;
    }
    g_output->apply(cmd);

    // Sync portal session: см. подробный комментарий в onBambuPrinterStatusUpdate.
    const bool  nowHeating = (cmd.mode == ControllerOutputMode::TargetTemperature);
    const float nowTarget  = cmd.targetTempC;
    const float dT         = nowTarget - g_moonrakerPrev.target;
    const bool  targetChanged = (dT > 0.01f) || (dT < -0.01f);
    const bool  changed = (g_moonrakerPrev.heating != nowHeating)
                       || (nowHeating && targetChanged);
    if (changed && g_moonrakerSession) {
        g_moonrakerSession(nowTarget, nowHeating);
    }
    g_moonrakerPrev.heating = nowHeating;
    g_moonrakerPrev.target  = nowTarget;

    if (g_logDecisions) {
        HAL_LOG_INFO("HEATER",
                     "VIRTUAL_CHAMBER: available=%d target=%.1f temp=%.1f hasSensor=%d → target=%.1f°C",
                     data.available ? 1 : 0,
                     data.target,
                     data.temperature,
                     data.hasSensor ? 1 : 0,
                     cmd.targetTempC);
    }
}

// Bambu Reader. Колбэк приходит только от активной интеграции — см. заметку
// у onVirtualChamberUpdate.
// Приоритеты:
//   1. gcode_state не PREPARE/RUNNING → OFF (печать не идёт).
//   2. printer.chamberTarget > 0 → setpoint от принтера (X1C с датчиком).
//   3. trayType из AMS → materialTempFromMenu (menu.mat_*).
//   4. Иначе OFF.
//
// Heat-allowed states (как присылает Bambu, заглавными):
//   RUNNING — печать идёт.
//   PREPARE — подготовка (прогрев / homing).
// Все остальные (IDLE, FINISH, FAILED, PAUSE, INIT, OFFLINE, SLICING,
// UNKNOWN, пустая строка) → OFF.
static bool bambuShouldHeat(const char* gcodeState) {
    if (!gcodeState || gcodeState[0] == '\0') return false;
    return strcmp(gcodeState, "RUNNING") == 0
        || strcmp(gcodeState, "PREPARE") == 0;
}

void onBambuPrinterStatusUpdate(void* /*ctx*/, const idryer::cloud::BambuPrinterStatus& status) {
    if (!g_output) return;

    const bool stateAllowsHeat = bambuShouldHeat(status.gcodeState);

    ControllerOutputCommand cmd{};
    float menuTemp = 0.0f;
    const char* source = "off";

    if (!stateAllowsHeat) {
        cmd.mode = ControllerOutputMode::Off;
        cmd.targetTempC = 0.0f;
        source = "state-not-printing";
    } else if (status.chamberTarget > 0.0f) {
        cmd.mode = ControllerOutputMode::TargetTemperature;
        cmd.targetTempC = status.chamberTarget;
        source = "printer";
    } else if (status.trayType[0] != '\0') {
        menuTemp = materialTempFromMenu(status.trayType);
        if (menuTemp > 0.0f) {
            cmd.mode = ControllerOutputMode::TargetTemperature;
            cmd.targetTempC = menuTemp;
            source = "menu";
        } else {
            cmd.mode = ControllerOutputMode::Off;
            cmd.targetTempC = 0.0f;
            source = "menu=0";
        }
    } else {
        cmd.mode = ControllerOutputMode::Off;
        cmd.targetTempC = 0.0f;
        source = "no-target";
    }

    g_output->apply(cmd);

    // Sync portal session: при смене heating/target дёргаем колбэк, который
    // обновит device().status.mode[] (Drying/Idle), sessionNum, targetTempC и
    // позовёт publishStatusNow. Без этого нагрев от Bambu не виден на портале
    // как сессия — нет push, нет истории.
    const bool  nowHeating = (cmd.mode == ControllerOutputMode::TargetTemperature);
    const float nowTarget  = cmd.targetTempC;
    const float dT         = nowTarget - g_bambuPrev.target;
    const bool  targetChanged = (dT > 0.01f) || (dT < -0.01f);
    const bool  changed = (g_bambuPrev.heating != nowHeating)
                       || (nowHeating && targetChanged);
    if (changed && g_bambuSession) {
        g_bambuSession(nowTarget, nowHeating);
    }
    g_bambuPrev.heating = nowHeating;
    g_bambuPrev.target  = nowTarget;

    if (g_logDecisions) {
        HAL_LOG_INFO("HEATER",
                     "BAMBU status: state=%s chamberTarget=%.1f chamberTemp=%.1f tray=%s menu=%.1f → target=%.1f°C (src=%s)",
                     status.gcodeState,
                     status.chamberTarget,
                     status.chamberTemp,
                     status.trayType,
                     menuTemp,
                     cmd.targetTempC,
                     source);
    }
}

} // namespace iheaterlink

#endif // ESP32 || ESP_PLATFORM
