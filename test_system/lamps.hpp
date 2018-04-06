#include <stdint.h>

/* This file was automatically generated.  Do not edit! */
volatile void lampsWork();
volatile void lampsFlash();
void lamp_red_off();
void lamp_red_on();
void lamp_wht_off();
void lamp_wht_on();
extern volatile uint8_t lamps_state_cnt;
extern volatile uint8_t lamps_state;
extern volatile uint32_t lamps_cnt;
extern volatile uint8_t lamps_cmd;
