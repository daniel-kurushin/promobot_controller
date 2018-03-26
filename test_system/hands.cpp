#include <stdio.h>
#include "hands.hpp"
// #include "pindefines.hpp"
#include "Arduino.h"

int lft_hand_limit = 0;
int rgt_hand_limit = 0;
int lft_hand_spin = 0;
int rgt_hand_spin = 0;
int lft_hand_cnt = 0;
int rgt_hand_cnt = 0;

uint8_t hands_last_cmd = 0;
int lft_hand_direction = 0; // 0 (up) or 1 (down)
int rgt_hand_direction = 0; // 0 (up) or 1 (down)
int hands_pwm = 2;

void processHands(char *resp_buf, int cmd)
{
	switch (cmd)
	{
		case 10: // get state
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d, lft_hand_spin: %d, lft_hand_direction: %d, rgt_hand_spin: %d, rgt_hand_direction: %d",
			                   hands_last_cmd,     state,     lft_hand_spin,     lft_hand_direction,     rgt_hand_spin,     rgt_hand_direction);
			break;
		case 11: // right up
			rgt_hand_direction = 0;
			rgt_hand_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, state);
			break;
		case 12: // right down
			rgt_hand_direction = 1;
			rgt_hand_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, state);
			break;
		case 13: // left up
			lft_hand_direction = 1;
			lft_hand_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, state);
			break;
		case 14: // left down
			lft_hand_direction = 0;		
			lft_hand_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			hands_last_cmd = cmd;
			sprintf(resp_buf, "hands_last_cmd: %d, state: %d", hands_last_cmd, state);
			break;
	}
}

void handsWork()
{
	if (hands_last_cmd == 11 | hands_last_cmd == 12)
	{
		if (!lft_hand_limit & (lft_hand_cnt < 1000))
		{
			lft_hand_spin = 1;
			lft_hand_cnt += hands_pwm;
			Serial.println(lft_hand_cnt);
		} else {
			lft_hand_limit = 1;
			lft_hand_spin = 0;
			lft_hand_cnt = 0;
		}
	}
	if (hands_last_cmd == 13 | hands_last_cmd == 14)
	{
		if (!rgt_hand_limit & (rgt_hand_cnt < 1000))
		{
			rgt_hand_spin = 1;
			rgt_hand_cnt += hands_pwm;
			Serial.println(rgt_hand_cnt);
		} else {
			rgt_hand_limit = 1;
			rgt_hand_spin = 0;
			rgt_hand_cnt = 0;
		}
	}
}


