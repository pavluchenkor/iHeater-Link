// Auto-generated. Do not edit.
#include <stddef.h>
#include "menu_ids.h"
#include "menu_types.h"
#include "menu_state.h"

#ifdef __cplusplus
extern "C" {
#endif
void heat_start(void);
void heat_stop(void);
#ifdef __cplusplus
}
#endif

extern MenuState menu;

const MenuItem g_menu[MENU__COUNT] = {
  [0] = {
    MENU_ROOT, { "IHEATER LINK", "IHEATER LINK" }, { nullptr, nullptr },
    MN_SUBMENU, -1, 1, 6,
    { { NULL }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [1] = {
    MENU_MATERIALS, { "МАТЕРИАЛЫ", "MATERIALS" }, { nullptr, nullptr },
    MN_SUBMENU, 0, 2, 26,
    { { NULL }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [2] = {
    MENU_IHEATER_LINK_PLA_TEMP, { "PLA", "PLA" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pla, 30, 90, 1, nullptr, false } },
    -1, 0
  },
  [3] = {
    MENU_IHEATER_LINK_PETG_TEMP, { "PETG", "PETG" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_petg, 30, 90, 1, nullptr, false } },
    -1, 0
  },
  [4] = {
    MENU_IHEATER_LINK_ABS_TEMP, { "ABS", "ABS" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_abs, 30, 90, 1, nullptr, false } },
    -1, 0
  },
  [5] = {
    MENU_IHEATER_LINK_ASA_TEMP, { "ASA", "ASA" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_asa, 30, 90, 1, nullptr, false } },
    -1, 0
  },
  [6] = {
    MENU_IHEATER_LINK_PC_TEMP, { "PC", "PC" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pc, 30, 90, 1, nullptr, false } },
    -1, 0
  },
  [7] = {
    MENU_IHEATER_LINK_PA_TEMP, { "PA / NYLON", "PA / NYLON" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pa, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [8] = {
    MENU_IHEATER_LINK_PA6_TEMP, { "PA6", "PA6" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pa6, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [9] = {
    MENU_IHEATER_LINK_PA11_TEMP, { "PA11", "PA11" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pa11, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [10] = {
    MENU_IHEATER_LINK_PA12_TEMP, { "PA12", "PA12" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pa12, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [11] = {
    MENU_IHEATER_LINK_PA66_TEMP, { "PA66", "PA66" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pa66, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [12] = {
    MENU_IHEATER_LINK_PMMA_TEMP, { "PMMA", "PMMA" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pmma, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [13] = {
    MENU_IHEATER_LINK_POM_TEMP, { "POM", "POM" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pom, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [14] = {
    MENU_IHEATER_LINK_PP_TEMP, { "PP", "PP" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pp, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [15] = {
    MENU_IHEATER_LINK_PS_TEMP, { "PS", "PS" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_ps, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [16] = {
    MENU_IHEATER_LINK_PVDF_TEMP, { "PVDF", "PVDF" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pvdf, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [17] = {
    MENU_IHEATER_LINK_TPI_TEMP, { "TPI", "TPI" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_tpi, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [18] = {
    MENU_IHEATER_LINK_PBT_TEMP, { "PBT", "PBT" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pbt, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [19] = {
    MENU_IHEATER_LINK_PPE_TEMP, { "PPE", "PPE" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_ppe, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [20] = {
    MENU_IHEATER_LINK_PEI_TEMP, { "PEI", "PEI" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pei, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [21] = {
    MENU_IHEATER_LINK_PEKK_TEMP, { "PEKK", "PEKK" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pekk, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [22] = {
    MENU_IHEATER_LINK_PES_TEMP, { "PES", "PES" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pes, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [23] = {
    MENU_IHEATER_LINK_PPA_TEMP, { "PPA", "PPA" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_ppa, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [24] = {
    MENU_IHEATER_LINK_PPS_TEMP, { "PPS", "PPS" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_pps, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [25] = {
    MENU_IHEATER_LINK_PEEK_TEMP, { "PEEK", "PEEK" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_peek, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [26] = {
    MENU_IHEATER_LINK_PPSU_TEMP, { "PPSU", "PPSU" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_ppsu, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [27] = {
    MENU_IHEATER_LINK_PSU_TEMP, { "PSU", "PSU" }, { "°C", "°C" },
    MN_VALUE, 1, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.mat_psu, 30, 200, 1, nullptr, false } },
    -1, 0
  },
  [28] = {
    MENU_HEATING, { "НАГРЕВ", "HEATING" }, { nullptr, nullptr },
    MN_SUBMENU, 0, 29, 4,
    { { NULL }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [29] = {
    MENU_HEAT_TEMP, { "ТЕМПЕРАТУРА", "TEMPERATURE" }, { "°C", "°C" },
    MN_VALUE, 28, -1, 0,
    { { NULL }, { VT_F32, (void*)&menu.heat_temp, 45, 65, 1, nullptr, false } },
    -1, 0
  },
  [30] = {
    MENU_HEAT_DURATION, { "ВРЕМЯ", "TIME" }, { "мин", "min" },
    MN_VALUE, 28, -1, 0,
    { { NULL }, { VT_U16, (void*)&menu.heat_duration, 0, 1440, 1, nullptr, false } },
    -1, 0
  },
  [31] = {
    MENU_HEAT_START, { "ВКЛЮЧИТЬ", "HEAT START" }, { nullptr, nullptr },
    MN_ACTION, 28, -1, 0,
    { { heat_start }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [32] = {
    MENU_HEAT_STOP, { "ВЫКЛЮЧИТЬ", "HEAT STOP" }, { nullptr, nullptr },
    MN_ACTION, 28, -1, 0,
    { { heat_stop }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [33] = {
    MENU_LOGS, { "ЛОГИ", "LOGS" }, { nullptr, nullptr },
    MN_SUBMENU, 0, 34, 4,
    { { NULL }, { VT_F32, NULL, 0, 0, 0, NULL, false } },
    -1, 0
  },
  [34] = {
    MENU_LOG_PORTAL_ENABLED, { "ПОРТАЛ", "PORTAL" }, { nullptr, nullptr },
    MN_TOGGLE, 33, -1, 0,
    { { NULL }, { VT_BOOL, (void*)&menu.log_portal, 0, 0, 1, nullptr, false } },
    -1, 0
  },
  [35] = {
    MENU_LOG_PRINTER_ENABLED, { "ПРИНТЕР", "PRINTER" }, { nullptr, nullptr },
    MN_TOGGLE, 33, -1, 0,
    { { NULL }, { VT_BOOL, (void*)&menu.log_printer, 0, 0, 1, nullptr, false } },
    -1, 0
  },
  [36] = {
    MENU_LOG_DEVICE_ENABLED, { "УСТРОЙСТВО", "DEVICE" }, { nullptr, nullptr },
    MN_TOGGLE, 33, -1, 0,
    { { NULL }, { VT_BOOL, (void*)&menu.log_device, 0, 0, 1, nullptr, false } },
    -1, 0
  },
  [37] = {
    MENU_LOG_DEBUG_ENABLED, { "DEBUG", "DEBUG" }, { nullptr, nullptr },
    MN_TOGGLE, 33, -1, 0,
    { { NULL }, { VT_BOOL, (void*)&menu.log_debug, 0, 0, 1, nullptr, false } },
    -1, 0
  },
  [38] = {
    MENU_IGNORE_EXTERNAL_CMD, { "ИГНОР. ВНЕШ. КОМАНД", "IGNOR EXT CMD" }, { nullptr, nullptr },
    MN_TOGGLE, 0, -1, 0,
    { { NULL }, { VT_BOOL, (void*)&menu.ign_ext_cmd, 0, 0, 1, nullptr, false } },
    -1, 0
  },
  [39] = {
    MENU_UNITS_COUNT, { "КОЛ-ВО ЮНИТОВ", "UNITS" }, { nullptr, nullptr },
    MN_VALUE, 0, -1, 0,
    { { NULL }, { VT_U8, (void*)&menu.units_count, 1, 1, 1, nullptr, false } },
    -1, 0
  },
  [40] = {
    MENU_LANGUAGE, { "ЯЗЫК", "LANGUAGE" }, { nullptr, nullptr },
    MN_VALUE, 0, -1, 0,
    { { NULL }, { VT_U8, (void*)&menu.language, 0, 1, 1, nullptr, false } },
    -1, 0
  },
};
