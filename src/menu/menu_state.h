// Auto-generated. Do not edit.
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "menu_ids.h"

#ifndef NUM_UNITS
#define NUM_UNITS 1
#endif

class MenuState {
public:
  float mat_pla = 45.0f;
  float mat_petg = 50.0f;
  float mat_abs = 60.0f;
  float mat_asa = 65.0f;
  float mat_pc = 70.0f;
  float mat_pa = 70.0f;
  float mat_pa6 = 60.0f;
  float mat_pa11 = 50.0f;
  float mat_pa12 = 50.0f;
  float mat_pa66 = 70.0f;
  float mat_pmma = 50.0f;
  float mat_pom = 50.0f;
  float mat_pp = 50.0f;
  float mat_ps = 50.0f;
  float mat_pvdf = 50.0f;
  float mat_tpi = 50.0f;
  float mat_pbt = 50.0f;
  float mat_ppe = 80.0f;
  float mat_pei = 100.0f;
  float mat_pekk = 100.0f;
  float mat_pes = 100.0f;
  float mat_ppa = 100.0f;
  float mat_pps = 100.0f;
  float mat_peek = 110.0f;
  float mat_ppsu = 120.0f;
  float mat_psu = 120.0f;
  float heat_temp = 60.0f;
  uint16_t heat_duration = (uint16_t)0;
  bool log_portal = false;
  bool log_printer = false;
  bool log_device = false;
  bool log_debug = false;
  bool ign_ext_cmd = false;
  uint8_t units_count = (uint8_t)1;
  uint8_t language = (uint8_t)1;

  void initDefaults();   // выставить дефолты из YAML
  void loadFromNVS();    // подхватить значения из NVS
  void saveToNVS();      // записать все поля в NVS (создать namespace)
};

extern MenuState menu;