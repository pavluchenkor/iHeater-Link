#pragma once

// Колбэки от LinkIntegrationsManager (Moonraker VIRTUAL_CHAMBER, Bambu Reader)
// → применение target температуры на RmtOutputAdapter.
//
// Колбэк приходит только от активной интеграции: неактивный клиент выключен
// менеджером, поэтому отдельного гейта нет.
//   - VirtualChamber: target>0 + available → ON, иначе OFF;
//   - Bambu: chamberTarget > 0 (printer) → ON; иначе trayType → menu.mat_*; иначе OFF.

namespace idryer { namespace cloud {
struct VirtualChamberData;
struct BambuPrinterStatus;
}}

namespace iheaterlink {

class RmtOutputAdapter;

/// Привязать модуль авто-нагрева к выходному адаптеру. Один раз при старте.
void wireAutoHeat(RmtOutputAdapter* output);

/// Колбэк смены сессии от auto-heat (Bambu или Moonraker): вызывается ТОЛЬКО
/// при изменении целевой температуры или флага heating (Drying/Idle).
/// Используется в main.cpp для синхронизации device().status.mode[]
/// (открытие/закрытие сессии сушки → portal status, sessionNum,
/// push-уведомления, история).
using SessionCallback = void(*)(float targetTempC, bool heating);
void wireBambuSession(SessionCallback cb);
void wireMoonrakerSession(SessionCallback cb);

/// Чтение опубликованного состояния нагрева (device().status): какая цель и
/// идёт ли нагрев прямо сейчас по данным портала.
///
/// Сторож дедупа сравнивает решение интеграции с опубликованным состоянием, а
/// не с прошлым решением самой интеграции. Иначе ручная команда, влезшая между
/// двумя обновлениями принтера, оставляет свою цель в status: железо уже греет
/// по принтеру, а карточка и Home Assistant показывают введённое человеком —
/// и так до тех пор, пока принтер сам не сменит цель.
using PublishedStateReader = void(*)(float& targetTempC, bool& heating);
void wirePublishedState(PublishedStateReader reader);

/// Включить/выключить логирование решений о нагреве (теги HEATER).
void setLogDecisions(bool enabled);

/// Колбэк, регистрируемый в LinkIntegrationsManager::setVirtualChamberCallback.
void onVirtualChamberUpdate(void* ctx, const idryer::cloud::VirtualChamberData& data);

/// Колбэк, регистрируемый в LinkIntegrationsManager::setBambuPrinterStatusCallback.
void onBambuPrinterStatusUpdate(void* ctx, const idryer::cloud::BambuPrinterStatus& status);

} // namespace iheaterlink
