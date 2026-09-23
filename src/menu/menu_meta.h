// Auto-generated for ESP32 LINK. Do not edit.
// Contains menu metadata only (no pointers to data or callbacks).
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MENU_META_COUNT 41
#define MENU_LANG_COUNT 2
#define MENU_SERIALIZED_MAX_SIZE 5799

typedef enum {
    META_SUBMENU = 0,
    META_ACTION = 1,
    META_VALUE = 2,
    META_TOGGLE = 3
} MenuMetaType;

typedef enum {
    META_VT_F32 = 0,
    META_VT_U16 = 1,
    META_VT_U8 = 2,
    META_VT_I32 = 3,
    META_VT_BOOL = 4,
    META_VT_U32 = 5
} MenuMetaValueType;

typedef enum {
    META_SCOPE_GLOBAL = 0,
    META_SCOPE_PER_UNIT = 1
} MenuMetaScope;

typedef struct {
    uint16_t id;
    const char* title[MENU_LANG_COUNT];
    const char* unit[MENU_LANG_COUNT];
    MenuMetaType type;
    int16_t parent;
    int16_t first_child;
    uint16_t child_count;
    MenuMetaValueType vtype;
    float min_val;
    float max_val;
    float step;
    MenuMetaScope scope;
    // menu_protocol_v1: канонические роли и хардкод-виджеты для портала.
    // role — стабильное имя из canonical_roles в mqtt_contract.yaml.
    // widget — override дефолтного UI-компонента (ProfileEditor / RfidWriter / LedPulse).
    // Оба nullptr для приватных пунктов меню (не публикуются на портал).
    const char* role;
    const char* widget;
} MenuMeta;

static const MenuMeta g_menu_meta[MENU_META_COUNT] = {
    // [0] root
    { 0, { "IHEATER LINK", "IHEATER LINK" }, { nullptr, nullptr },
      META_SUBMENU, -1, 1, 6,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [1] materials
    { 1, { "МАТЕРИАЛЫ", "MATERIALS" }, { nullptr, nullptr },
      META_SUBMENU, 0, 2, 26,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [2] iheater_link_pla_temp
    { 2, { "PLA", "PLA" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [3] iheater_link_petg_temp
    { 3, { "PETG", "PETG" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [4] iheater_link_abs_temp
    { 4, { "ABS", "ABS" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [5] iheater_link_asa_temp
    { 5, { "ASA", "ASA" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [6] iheater_link_pc_temp
    { 6, { "PC", "PC" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [7] iheater_link_pa_temp
    { 7, { "PA / NYLON", "PA / NYLON" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [8] iheater_link_pa6_temp
    { 8, { "PA6", "PA6" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [9] iheater_link_pa11_temp
    { 9, { "PA11", "PA11" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [10] iheater_link_pa12_temp
    { 10, { "PA12", "PA12" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [11] iheater_link_pa66_temp
    { 11, { "PA66", "PA66" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [12] iheater_link_pmma_temp
    { 12, { "PMMA", "PMMA" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [13] iheater_link_pom_temp
    { 13, { "POM", "POM" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [14] iheater_link_pp_temp
    { 14, { "PP", "PP" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [15] iheater_link_ps_temp
    { 15, { "PS", "PS" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [16] iheater_link_pvdf_temp
    { 16, { "PVDF", "PVDF" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [17] iheater_link_tpi_temp
    { 17, { "TPI", "TPI" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [18] iheater_link_pbt_temp
    { 18, { "PBT", "PBT" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [19] iheater_link_ppe_temp
    { 19, { "PPE", "PPE" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [20] iheater_link_pei_temp
    { 20, { "PEI", "PEI" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [21] iheater_link_pekk_temp
    { 21, { "PEKK", "PEKK" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [22] iheater_link_pes_temp
    { 22, { "PES", "PES" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [23] iheater_link_ppa_temp
    { 23, { "PPA", "PPA" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [24] iheater_link_pps_temp
    { 24, { "PPS", "PPS" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [25] iheater_link_peek_temp
    { 25, { "PEEK", "PEEK" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [26] iheater_link_ppsu_temp
    { 26, { "PPSU", "PPSU" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [27] iheater_link_psu_temp
    { 27, { "PSU", "PSU" }, { "°C", "°C" },
      META_VALUE, 1, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [28] heating
    { 28, { "НАГРЕВ", "HEATING" }, { nullptr, nullptr },
      META_SUBMENU, 0, 29, 4,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [29] heat_temp
    { 29, { "ТЕМПЕРАТУРА", "TEMPERATURE" }, { "°C", "°C" },
      META_VALUE, 28, -1, 0,
      META_VT_F32, 45.0f, 65.0f, 1.0f, META_SCOPE_GLOBAL,
      "iheater.heat_temp", nullptr },
    // [30] heat_duration
    { 30, { "ВРЕМЯ", "TIME" }, { "мин", "min" },
      META_VALUE, 28, -1, 0,
      META_VT_U16, 0.0f, 1440.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [31] heat_start
    { 31, { "ВКЛЮЧИТЬ", "HEAT START" }, { nullptr, nullptr },
      META_ACTION, 28, -1, 0,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      "iheater.heat_start", "HeaterControl" },
    // [32] heat_stop
    { 32, { "ВЫКЛЮЧИТЬ", "HEAT STOP" }, { nullptr, nullptr },
      META_ACTION, 28, -1, 0,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      "iheater.heat_stop", nullptr },
    // [33] logs
    { 33, { "ЛОГИ", "LOGS" }, { nullptr, nullptr },
      META_SUBMENU, 0, 34, 4,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [34] log_portal_enabled
    { 34, { "ПОРТАЛ", "PORTAL" }, { nullptr, nullptr },
      META_TOGGLE, 33, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [35] log_printer_enabled
    { 35, { "ПРИНТЕР", "PRINTER" }, { nullptr, nullptr },
      META_TOGGLE, 33, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [36] log_device_enabled
    { 36, { "УСТРОЙСТВО", "DEVICE" }, { nullptr, nullptr },
      META_TOGGLE, 33, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [37] log_debug_enabled
    { 37, { "DEBUG", "DEBUG" }, { nullptr, nullptr },
      META_TOGGLE, 33, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [38] ignore_external_cmd
    { 38, { "ИГНОР. ВНЕШ. КОМАНД", "IGNOR EXT CMD" }, { nullptr, nullptr },
      META_TOGGLE, 0, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      "system.ignore_external_cmd", nullptr },
    // [39] units_count
    { 39, { "КОЛ-ВО ЮНИТОВ", "UNITS" }, { nullptr, nullptr },
      META_VALUE, 0, -1, 0,
      META_VT_U8, 1.0f, 1.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [40] language
    { 40, { "ЯЗЫК", "LANGUAGE" }, { nullptr, nullptr },
      META_VALUE, 0, -1, 0,
      META_VT_U8, 0.0f, 1.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
};

static inline const MenuMeta* menu_meta_get(uint16_t id) {
    if (id < MENU_META_COUNT) return &g_menu_meta[id];
    return nullptr;
}
