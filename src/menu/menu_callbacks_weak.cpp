// AUTO-GENERATED. DO NOT EDIT.
// Weak stubs for menu callbacks so linking always succeeds.
#include <Arduino.h>
#ifdef __cplusplus
extern "C" {
#endif
void heat_start(void) __attribute__((weak));
void heat_start(void) { /* stub */ }
void heat_stop(void) __attribute__((weak));
void heat_stop(void) { /* stub */ }
uint8_t menu_get_active_controller(void) __attribute__((weak));
uint8_t menu_get_active_controller(void) { return 0; }
#ifdef __cplusplus
} // extern "C"
#endif