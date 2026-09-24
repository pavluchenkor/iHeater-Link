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
// Прошлое решение интеграции. Используется как запасной источник сравнения,
// пока не подключён читатель опубликованного состояния (wirePublishedState).
struct SessionPrev {
    bool  heating = false;
    float target  = 0.0f;
};
SessionPrev g_bambuPrev;
SessionPrev g_moonrakerPrev;
PublishedStateReader g_published = nullptr;

// Нужна ли синхронизация портального статуса с решением интеграции.
//
// Сравнение идёт с ОПУБЛИКОВАННЫМ состоянием, а не с прошлым решением самой
// интеграции: ручная команда (меню, действие карточки) пишет в status свою
// цель, а железу её тут же возвращает следующий push принтера. Сравнение с
// прошлым решением интеграции такой перезаписи не видит — 55 равны 55 — и
// статус остаётся с ручными 60, пока принтер сам не сменит цель.
//
// Дедуп при этом сохраняется: пока опубликованное совпадает с решением,
// колбэк не зовётся, и push ~1 Гц от Bambu не засоряет MQTT портала.
// @p prev — запасной источник, когда читатель не подключён.
bool sessionNeedsSync(SessionPrev& prev, bool nowHeating, float nowTarget) {
    float pubTarget  = prev.target;
    bool  pubHeating = prev.heating;
    if (g_published) g_published(pubTarget, pubHeating);

    const float dT = nowTarget - pubTarget;
    const bool  targetChanged = (dT > 0.01f) || (dT < -0.01f);
    const bool  changed = (pubHeating != nowHeating) || (nowHeating && targetChanged);

    prev.heating = nowHeating;
    prev.target  = nowTarget;
    return changed;
}
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

void wirePublishedState(PublishedStateReader reader) {
    g_published = reader;
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
    const bool nowHeating = (cmd.mode == ControllerOutputMode::TargetTemperature);
    if (sessionNeedsSync(g_moonrakerPrev, nowHeating, cmd.targetTempC) && g_moonrakerSession) {
        g_moonrakerSession(cmd.targetTempC, nowHeating);
    }

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
    const bool nowHeating = (cmd.mode == ControllerOutputMode::TargetTemperature);
    if (sessionNeedsSync(g_bambuPrev, nowHeating, cmd.targetTempC) && g_bambuSession) {
        g_bambuSession(cmd.targetTempC, nowHeating);
    }

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
