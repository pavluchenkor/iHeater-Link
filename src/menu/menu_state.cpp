// Auto-generated. Do not edit.
#include <string.h>
#include <stdio.h>
#include "menu_state.h"
#include "menu_types.h"
#include "menu_nvs.h"
#include "menu_nvs_io.h"

MenuState menu;

void MenuState::initDefaults(){
  this->mat_pla = 45.0f;
  this->mat_petg = 50.0f;
  this->mat_abs = 60.0f;
  this->mat_asa = 65.0f;
  this->mat_pc = 70.0f;
  this->mat_pa = 70.0f;
  this->mat_pa6 = 60.0f;
  this->mat_pa11 = 50.0f;
  this->mat_pa12 = 50.0f;
  this->mat_pa66 = 70.0f;
  this->mat_pmma = 50.0f;
  this->mat_pom = 50.0f;
  this->mat_pp = 50.0f;
  this->mat_ps = 50.0f;
  this->mat_pvdf = 50.0f;
  this->mat_tpi = 50.0f;
  this->mat_pbt = 50.0f;
  this->mat_ppe = 80.0f;
  this->mat_pei = 100.0f;
  this->mat_pekk = 100.0f;
  this->mat_pes = 100.0f;
  this->mat_ppa = 100.0f;
  this->mat_pps = 100.0f;
  this->mat_peek = 110.0f;
  this->mat_ppsu = 120.0f;
  this->mat_psu = 120.0f;
  this->heat_temp = 60.0f;
  this->heat_duration = (uint16_t)0;
  this->log_portal = false;
  this->log_printer = false;
  this->log_device = false;
  this->log_debug = false;
  this->ign_ext_cmd = false;
  this->units_count = (uint8_t)1;
  this->language = (uint8_t)1;
}

void MenuState::loadFromNVS(){
  menu_nvs_begin();
  uint32_t magic = 0, ver = 0;
  ee_read(NVS_KEY_MAGIC, magic);
  ee_read(NVS_KEY_VERSION, ver);
  if (magic != NVS_MENU_MAGIC || ver != (uint32_t)NVS_MENU_VERSION) {
    menu_nvs_end();
    saveToNVS();  // first boot: persist defaults + magic
    return;
  }
  char key[16];
  (void)key;
  ee_read("mat_pla", this->mat_pla);
  ee_read("mat_petg", this->mat_petg);
  ee_read("mat_abs", this->mat_abs);
  ee_read("mat_asa", this->mat_asa);
  ee_read("mat_pc", this->mat_pc);
  ee_read("mat_pa", this->mat_pa);
  ee_read("mat_pa6", this->mat_pa6);
  ee_read("mat_pa11", this->mat_pa11);
  ee_read("mat_pa12", this->mat_pa12);
  ee_read("mat_pa66", this->mat_pa66);
  ee_read("mat_pmma", this->mat_pmma);
  ee_read("mat_pom", this->mat_pom);
  ee_read("mat_pp", this->mat_pp);
  ee_read("mat_ps", this->mat_ps);
  ee_read("mat_pvdf", this->mat_pvdf);
  ee_read("mat_tpi", this->mat_tpi);
  ee_read("mat_pbt", this->mat_pbt);
  ee_read("mat_ppe", this->mat_ppe);
  ee_read("mat_pei", this->mat_pei);
  ee_read("mat_pekk", this->mat_pekk);
  ee_read("mat_pes", this->mat_pes);
  ee_read("mat_ppa", this->mat_ppa);
  ee_read("mat_pps", this->mat_pps);
  ee_read("mat_peek", this->mat_peek);
  ee_read("mat_ppsu", this->mat_ppsu);
  ee_read("mat_psu", this->mat_psu);
  ee_read("heat_temp", this->heat_temp);
  ee_read("heat_duration", this->heat_duration);
  ee_read("log_portal", this->log_portal);
  ee_read("log_printer", this->log_printer);
  ee_read("log_device", this->log_device);
  ee_read("log_debug", this->log_debug);
  ee_read("ign_ext_cmd", this->ign_ext_cmd);
  ee_read("units_count", this->units_count);
  ee_read("language", this->language);
  menu_nvs_end();
}

void MenuState::saveToNVS(){
  menu_nvs_begin();
  ee_write(NVS_KEY_MAGIC,   (uint32_t)NVS_MENU_MAGIC);
  ee_write(NVS_KEY_VERSION, (uint32_t)NVS_MENU_VERSION);
  char key[16];
  (void)key;
  ee_store_field("mat_pla", this->mat_pla);
  ee_store_field("mat_petg", this->mat_petg);
  ee_store_field("mat_abs", this->mat_abs);
  ee_store_field("mat_asa", this->mat_asa);
  ee_store_field("mat_pc", this->mat_pc);
  ee_store_field("mat_pa", this->mat_pa);
  ee_store_field("mat_pa6", this->mat_pa6);
  ee_store_field("mat_pa11", this->mat_pa11);
  ee_store_field("mat_pa12", this->mat_pa12);
  ee_store_field("mat_pa66", this->mat_pa66);
  ee_store_field("mat_pmma", this->mat_pmma);
  ee_store_field("mat_pom", this->mat_pom);
  ee_store_field("mat_pp", this->mat_pp);
  ee_store_field("mat_ps", this->mat_ps);
  ee_store_field("mat_pvdf", this->mat_pvdf);
  ee_store_field("mat_tpi", this->mat_tpi);
  ee_store_field("mat_pbt", this->mat_pbt);
  ee_store_field("mat_ppe", this->mat_ppe);
  ee_store_field("mat_pei", this->mat_pei);
  ee_store_field("mat_pekk", this->mat_pekk);
  ee_store_field("mat_pes", this->mat_pes);
  ee_store_field("mat_ppa", this->mat_ppa);
  ee_store_field("mat_pps", this->mat_pps);
  ee_store_field("mat_peek", this->mat_peek);
  ee_store_field("mat_ppsu", this->mat_ppsu);
  ee_store_field("mat_psu", this->mat_psu);
  ee_store_field("heat_temp", this->heat_temp);
  ee_store_field("heat_duration", this->heat_duration);
  ee_store_field("log_portal", this->log_portal);
  ee_store_field("log_printer", this->log_printer);
  ee_store_field("log_device", this->log_device);
  ee_store_field("log_debug", this->log_debug);
  ee_store_field("ign_ext_cmd", this->ign_ext_cmd);
  ee_store_field("units_count", this->units_count);
  ee_store_field("language", this->language);
  menu_nvs_end();
}