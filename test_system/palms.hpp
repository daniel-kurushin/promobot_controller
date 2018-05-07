#include <stdint.h>

/* This file was automatically generated.  Do not edit! */
extern volatile uint8_t palms_cmd;
void processPalms(char *resp_buf,int palms_cmd);
void rgtPalmWork();
void lftPalmWork();
void palmsWork();
extern volatile uint8_t rgt_palm_state;
extern volatile uint8_t lft_palm_state;
extern volatile uint32_t palms_cnt;
