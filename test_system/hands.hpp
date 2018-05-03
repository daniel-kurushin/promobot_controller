#include "pindefines.hpp"
#include <stdio.h>

/* This file was automatically generated.  Do not edit! */

typedef enum HAND_STATE {
	HAND_STATE_INIT, // 0
	HAND_STATE_GO_DOWN, // 1
	HAND_STATE_SPIN_UP, // 2
	HAND_STATE_SPIN_DOWN, // 3
	HAND_STATE_BEGIN_SPIN_UP, // 4
	HAND_STATE_BEGIN_SPIN_DOWN, // 5
	HAND_STATE_STOP, //6
	HAND_STATE_SPIN_FAIL, // 7
	HAND_STATE_IDLE // 8
} State_t;
// void handsWork();
void leftHandWork();
void rigthHandWork();
void handsWork();
void RH_GO_UP();
void LH_GO_UP();
void RH_GO_DOWN();
void LH_GO_DOWN();
void processHands(char *resp_buf,int cmd);
extern int rgt_hand_direction;
extern int lft_hand_direction;
extern uint8_t hands_state;
extern uint8_t hands_last_cmd;
extern int rgt_hand_cnt;
extern int lft_hand_cnt;
extern int rgt_hand_spin;
extern int lft_hand_spin;
extern int rgt_hand_limit;
extern int lft_hand_limit;
extern volatile uint8_t hands_pwm;
extern volatile State_t old_LH_State,old_RH_State;
extern volatile uint16_t LH_Time,RH_Time;
extern volatile uint32_t LH_Command,old_LH_Command,RH_Command,old_RH_Command;
extern volatile State_t LH_State,RH_State;
