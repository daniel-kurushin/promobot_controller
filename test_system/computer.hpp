#include <stdio.h>

/* This file was automatically generated.  Do not edit! */
void processComp(char *resp_buf, uint8_t cmd);
void computerWork();
void comp_off();
void init_comp_relays();
void show_comp_error();
void lamp_red_off();
void lamp_red_on();
void lamp_wht_off();
void lamp_wht_on();
void lamps_init();
extern uint8_t sys;
volatile extern uint32_t comp_boot_time;
volatile extern uint16_t computer_state;
