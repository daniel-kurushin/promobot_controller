#include <stdio.h>
#include "hands.hpp"
// #include "pindefines.hpp"
#include "Arduino.h"

volatile State_t LH_State, RH_State = HAND_STATE_INIT;
volatile uint32_t LH_Command, old_LH_Command, RH_Command, old_RH_Command = 0;
volatile uint16_t LH_Time, RH_Time = 0;
volatile State_t old_LH_State, old_RH_State = NULL;
volatile uint8_t hands_pwm = 30;

int lft_hand_limit = 0;
int rgt_hand_limit = 0;
int lft_hand_spin = 0;
int rgt_hand_spin = 0;
int lft_hand_cnt = 0;
int rgt_hand_cnt = 0;

volatile uint8_t hands_cnt = 0;
volatile uint8_t mhcnt = 0;

uint8_t hands_last_cmd = 0;
uint8_t hands_state = 0;
int lft_hand_direction = 0; // 0 (up) or 1 (down)
int rgt_hand_direction = 0; // 0 (up) or 1 (down)
// int hands_pwm = 2;

void LH_GO_UP() {	PORTL |= _BV(LHUP); PORTL &= ~_BV(LHDOWN); PORTG |= _BV(LEFTHAND); }
void LH_GO_DOWN() {  PORTL |= _BV(LHDOWN); PORTL &= ~_BV(LHUP); PORTG |= _BV(LEFTHAND); }

void RH_GO_UP() {	PORTL |= _BV(RHUP); PORTL &= ~_BV(RHDOWN); PORTG |= _BV(RIGHTHAND); }
void RH_GO_DOWN() {  PORTL |= _BV(RHDOWN); PORTL &= ~_BV(RHUP); PORTG |= _BV(RIGHTHAND); }

void handsWork() 
{
	if (hands_cnt++ == 0)
	{
		leftHandWork();
		rigthHandWork();
		mhcnt = 0;
	}
	if (mhcnt++ > hands_pwm)
	{
		LH_STOP;
		RH_STOP;
	}
}
/// TODO: insert hands cmds switch somewhere !!!
// switch (cmd)
// {
// 	case LEFTHANDUP:
// 		LH_Command = LEFTHANDUP;
// 		break;
// 	case LEFTHANDDOWN:
// 		LH_Command = LEFTHANDDOWN;
// 		break;
// 	case LEFTHANDSTOP:
// 		RH_Command = LEFTHANDSTOP;
// 		break;
// 	case RIGHTHANDUP:
// 		RH_Command = RIGHTHANDUP;
// 		break;
// 	case RIGHTHANDDOWN:
// 		RH_Command = RIGHTHANDDOWN;
// 		break;
// 	case RIGHTHANDSTOP:
// 		RH_Command = RIGHTHANDSTOP;
// 		break;
// }



void processHands(char *resp_buf, int cmd)
{
	switch (cmd)
	{
		case 10: // get state
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d, lft_hand_spin: %d, lft_hand_direction: %d, rgt_hand_spin: %d, rgt_hand_direction: %d",
			                   hands_last_cmd,     hands_state,     lft_hand_spin,     lft_hand_direction,     rgt_hand_spin,     rgt_hand_direction);
			break;
		case 11: // right up
			RH_Command = RIGHTHANDUP;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
		case 12: // right down
			RH_Command = RIGHTHANDDOWN;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
		case 13: // left up
			LH_Command = LEFTHANDUP;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
		case 14: // left down
			LH_Command = LEFTHANDDOWN;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
		case 15: // right stop
			RH_Command = RIGHTHANDSTOP;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
		case 16: // left stop
			LH_Command = LEFTHANDSTOP;
			if (0) // error state
			{
				hands_state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, hands_state);
			break;
	}
	Serial.println(RH_Command);
}

int cnt = 0;
void rigthHandWork()
{
	// Serial.print(" RH: ");
	// Serial.print(RH_State);
	// Serial.println(RH_LIMIT);
	// if (cnt++ > 10)
	// {
		// Serial.println();
		// cnt = 0;
	// }
	switch (RH_State)
	{
		case HAND_STATE_INIT: // 0
			hands_pwm = 0;
			RH_Time = 0;
			RH_State = HAND_STATE_GO_DOWN;
			break;
		case HAND_STATE_GO_DOWN: // 1
			hands_pwm = HANDS_PWM_DN;
			old_RH_Command = RIGHTHANDDOWN;
			RH_GO_DOWN();
			if (RH_LIMIT || (++RH_Time > 1000)) RH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_DOWN: // 2
			hands_pwm = HANDS_PWM_DN;
			RH_GO_DOWN();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (!RH_LIMIT & (++RH_Time > 10)) RH_State = HAND_STATE_SPIN_DOWN;
			break;
		case HAND_STATE_SPIN_DOWN: // 3
			hands_pwm = HANDS_PWM_DN;
			RH_GO_DOWN();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (RH_LIMIT) RH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_UP: // 4
			hands_pwm = HANDS_PWM_UP;
			RH_GO_UP();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (RH_NOT_LIMIT & (++RH_Time > 10)) RH_State = HAND_STATE_SPIN_UP;
			break;
		case HAND_STATE_SPIN_UP: // 5
			hands_pwm = HANDS_PWM_UP;
			RH_GO_UP();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (RH_LIMIT) RH_State = HAND_STATE_STOP;
			break;
			// HAND_STATE_SPIN_UP ; HAND_STATE_SPIN_FAIL ; RH_Time > 10000
		case HAND_STATE_STOP: // 6
			hands_pwm = 0;
			RH_STOP;
			RH_Time = 0;
			RH_State = HAND_STATE_IDLE;
			break;
		case HAND_STATE_SPIN_FAIL: // 7
			hands_pwm = 0;
			RH_STOP;
			RH_Time = 0;
			break;
		case HAND_STATE_IDLE: // 8
			if ((RH_IS_NEW_CMD) & (RH_Command == RIGHTHANDUP))
			{
				RH_State = HAND_STATE_BEGIN_SPIN_UP;
				RH_Time = 0;
				old_RH_Command = RH_Command;
			}
			if ((RH_IS_NEW_CMD) & (RH_Command == RIGHTHANDDOWN))
			{
				RH_State = HAND_STATE_BEGIN_SPIN_DOWN;
				RH_Time = 0;
				old_RH_Command = RH_Command;
			}
			break;
	}
	old_RH_State = RH_State;
}


void leftHandWork()
{
	// Serial.print("LH: ");
	// Serial.println(LH_LIMIT);
	// Serial.print(" LH: ");
	// Serial.println(LH_Time);
	switch (LH_State)
	{
		case HAND_STATE_INIT:
			hands_pwm = 0;
			LH_Time = 0;
			LH_State = HAND_STATE_GO_DOWN;
			break;
		case HAND_STATE_GO_DOWN:
			hands_pwm = HANDS_PWM_DN;
			old_LH_Command = LEFTHANDDOWN;
			LH_GO_DOWN();
			if (LH_LIMIT || (++LH_Time > 1000)) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_DOWN:
			hands_pwm = HANDS_PWM_DN;
			LH_GO_DOWN();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_NOT_LIMIT & (++LH_Time > 10)) LH_State = HAND_STATE_SPIN_DOWN;
			break;
		case HAND_STATE_SPIN_DOWN:
			hands_pwm = HANDS_PWM_DN;
			LH_GO_DOWN();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_LIMIT) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_UP:
			hands_pwm = HANDS_PWM_UP;
			LH_GO_UP();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_NOT_LIMIT & (++LH_Time > 10)) LH_State = HAND_STATE_SPIN_UP;
			break;
		case HAND_STATE_SPIN_UP:
			hands_pwm = HANDS_PWM_UP;
			LH_GO_UP();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_LIMIT) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_STOP:
			hands_pwm = 0;
			LH_STOP;
			LH_Time = 0;
			LH_State = HAND_STATE_IDLE;
			break;
		case HAND_STATE_SPIN_FAIL:
			hands_pwm = 0;
			LH_STOP;
			LH_Time = 0;
			break;
		case HAND_STATE_IDLE:
			if ((LH_IS_NEW_CMD) & (LH_Command == LEFTHANDUP))
			{
				LH_State = HAND_STATE_BEGIN_SPIN_UP;
				LH_Time = 0;
				old_LH_Command = LH_Command;
			}
			if ((LH_IS_NEW_CMD) & (LH_Command == LEFTHANDDOWN))
			{
				LH_State = HAND_STATE_BEGIN_SPIN_DOWN;
				LH_Time = 0;
				old_LH_Command = LH_Command;
			}
			break;
	}
	old_LH_State = LH_State;
}

// void handsWork() // old
// {
// 	if (hands_last_cmd == 11 | hands_last_cmd == 12)
// 	{
// 		if (!lft_hand_limit & (lft_hand_cnt < 1000))
// 		{
// 			lft_hand_spin = 1;
// 			lft_hand_cnt += hands_pwm;
// 			Serial.println(lft_hand_cnt);
// 		} else {
// 			lft_hand_limit = 1;
// 			lft_hand_spin = 0;
// 			lft_hand_cnt = 0;
// 		}
// 	}
// 	if (hands_last_cmd == 13 | hands_last_cmd == 14)
// 	{
// 		if (!rgt_hand_limit & (rgt_hand_cnt < 1000))
// 		{
// 			rgt_hand_spin = 1;
// 			rgt_hand_cnt += hands_pwm;
// 			Serial.println(rgt_hand_cnt);
// 		} else {
// 			rgt_hand_limit = 1;
// 			rgt_hand_spin = 0;
// 			rgt_hand_cnt = 0;
// 		}
// 	}
// }


