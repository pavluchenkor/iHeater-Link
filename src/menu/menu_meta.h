// Auto-generated for ESP32 LINK. Do not edit.
// Contains menu metadata only (no pointers to data or callbacks).
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MENU_META_COUNT 47
#define MENU_LANG_COUNT 2
#define MENU_SERIALIZED_MAX_SIZE 6332

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
      META_SUBMENU, -1, 1, 8,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [1] portal
    { 1, { "ПОРТАЛ", "PORTAL" }, { nullptr, nullptr },
      META_SUBMENU, 0, 2, 1,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [2] start_claim
    { 2, { "СВЯЗАТЬ", "CLAIM" }, { nullptr, nullptr },
      META_ACTION, 1, -1, 0,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [3] connections
    { 3, { "ПОДКЛЮЧЕНИЯ", "CONNECTIONS" }, { nullptr, nullptr },
      META_SUBMENU, 0, 4, 3,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [4] bambu_enabled
    { 4, { "BAMBU", "BAMBU" }, { nullptr, nullptr },
      META_TOGGLE, 3, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      "iheater.bambu_enabled", nullptr },
    // [5] moon_enabled
    { 5, { "MOONRAKER", "MOONRAKER" }, { nullptr, nullptr },
      META_TOGGLE, 3, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      "iheater.moonraker_enabled", nullptr },
    // [6] ha_enabled
    { 6, { "HOME ASSISTANT", "HOME ASSISTANT" }, { nullptr, nullptr },
      META_TOGGLE, 3, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      "iheater.ha_enabled", nullptr },
    // [7] materials
    { 7, { "МАТЕРИАЛЫ", "MATERIALS" }, { nullptr, nullptr },
      META_SUBMENU, 0, 8, 26,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [8] iheater_link_pla_temp
    { 8, { "PLA", "PLA" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [9] iheater_link_petg_temp
    { 9, { "PETG", "PETG" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [10] iheater_link_abs_temp
    { 10, { "ABS", "ABS" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [11] iheater_link_asa_temp
    { 11, { "ASA", "ASA" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [12] iheater_link_pc_temp
    { 12, { "PC", "PC" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 90.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [13] iheater_link_pa_temp
    { 13, { "PA / NYLON", "PA / NYLON" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [14] iheater_link_pa6_temp
    { 14, { "PA6", "PA6" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [15] iheater_link_pa11_temp
    { 15, { "PA11", "PA11" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [16] iheater_link_pa12_temp
    { 16, { "PA12", "PA12" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [17] iheater_link_pa66_temp
    { 17, { "PA66", "PA66" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [18] iheater_link_pmma_temp
    { 18, { "PMMA", "PMMA" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [19] iheater_link_pom_temp
    { 19, { "POM", "POM" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [20] iheater_link_pp_temp
    { 20, { "PP", "PP" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [21] iheater_link_ps_temp
    { 21, { "PS", "PS" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [22] iheater_link_pvdf_temp
    { 22, { "PVDF", "PVDF" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [23] iheater_link_tpi_temp
    { 23, { "TPI", "TPI" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [24] iheater_link_pbt_temp
    { 24, { "PBT", "PBT" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [25] iheater_link_ppe_temp
    { 25, { "PPE", "PPE" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [26] iheater_link_pei_temp
    { 26, { "PEI", "PEI" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [27] iheater_link_pekk_temp
    { 27, { "PEKK", "PEKK" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [28] iheater_link_pes_temp
    { 28, { "PES", "PES" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [29] iheater_link_ppa_temp
    { 29, { "PPA", "PPA" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [30] iheater_link_pps_temp
    { 30, { "PPS", "PPS" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [31] iheater_link_peek_temp
    { 31, { "PEEK", "PEEK" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [32] iheater_link_ppsu_temp
    { 32, { "PPSU", "PPSU" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [33] iheater_link_psu_temp
    { 33, { "PSU", "PSU" }, { "°C", "°C" },
      META_VALUE, 7, -1, 0,
      META_VT_F32, 30.0f, 200.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [34] heating
    { 34, { "НАГРЕВ", "HEATING" }, { nullptr, nullptr },
      META_SUBMENU, 0, 35, 4,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [35] heat_temp
    { 35, { "ТЕМПЕРАТУРА", "TEMPERATURE" }, { "°C", "°C" },
      META_VALUE, 34, -1, 0,
      META_VT_F32, 45.0f, 65.0f, 1.0f, META_SCOPE_GLOBAL,
      "iheater.heat_temp", nullptr },
    // [36] heat_duration
    { 36, { "ВРЕМЯ", "TIME" }, { "мин", "min" },
      META_VALUE, 34, -1, 0,
      META_VT_U16, 0.0f, 1440.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [37] heat_start
    { 37, { "ВКЛЮЧИТЬ", "HEAT START" }, { nullptr, nullptr },
      META_ACTION, 34, -1, 0,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      "iheater.heat_start", "HeaterControl" },
    // [38] heat_stop
    { 38, { "ВЫКЛЮЧИТЬ", "HEAT STOP" }, { nullptr, nullptr },
      META_ACTION, 34, -1, 0,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      "iheater.heat_stop", nullptr },
    // [39] logs
    { 39, { "ЛОГИ", "LOGS" }, { nullptr, nullptr },
      META_SUBMENU, 0, 40, 4,
      META_VT_F32, 0.0f, 0.0f, 0.0f, META_SCOPE_PER_UNIT,
      nullptr, nullptr },
    // [40] log_portal_enabled
    { 40, { "ПОРТАЛ", "PORTAL" }, { nullptr, nullptr },
      META_TOGGLE, 39, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [41] log_printer_enabled
    { 41, { "ПРИНТЕР", "PRINTER" }, { nullptr, nullptr },
      META_TOGGLE, 39, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [42] log_device_enabled
    { 42, { "УСТРОЙСТВО", "DEVICE" }, { nullptr, nullptr },
      META_TOGGLE, 39, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [43] log_debug_enabled
    { 43, { "DEBUG", "DEBUG" }, { nullptr, nullptr },
      META_TOGGLE, 39, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [44] ignore_external_cmd
    { 44, { "ИГНОР. ВНЕШ. КОМАНД", "IGNOR EXT CMD" }, { nullptr, nullptr },
      META_TOGGLE, 0, -1, 0,
      META_VT_BOOL, 0.0f, 0.0f, 1.0f, META_SCOPE_GLOBAL,
      "system.ignore_external_cmd", nullptr },
    // [45] units_count
    { 45, { "КОЛ-ВО ЮНИТОВ", "UNITS" }, { nullptr, nullptr },
      META_VALUE, 0, -1, 0,
      META_VT_U8, 1.0f, 1.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
    // [46] language
    { 46, { "ЯЗЫК", "LANGUAGE" }, { nullptr, nullptr },
      META_VALUE, 0, -1, 0,
      META_VT_U8, 0.0f, 1.0f, 1.0f, META_SCOPE_GLOBAL,
      nullptr, nullptr },
};

static inline const MenuMeta* menu_meta_get(uint16_t id) {
    if (id < MENU_META_COUNT) return &g_menu_meta[id];
    return nullptr;
}
