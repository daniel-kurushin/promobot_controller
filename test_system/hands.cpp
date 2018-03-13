#include <stdio.h>
#include "hands.hpp"
#include "Arduino.h"

int hands_limit = 0;
int hands_spin = 0;
int hands_cnt = 0;

int hands_direction = 0; // 0 (up) or 1 (down)
int hands_pwm = 2;

void processHands(char *resp_buf, int cmd)
{
	switch (cmd)
	{
		case 10: // get state
			last_cmd = cmd;
			sprintf(resp_buf, "last_cmd: %d, state: %d, hands_spin: %d, hands_direction: %d",
			                   last_cmd,     state,     hands_spin,     hands_direction);
			break;
		case 11: // right up
			hands_direction = 0;
			hands_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			last_cmd = cmd;
			sprintf(resp_buf, "last_cmd: %d, state: %d", last_cmd, state);
			break;
		case 12: // right down
			hands_direction = 1;
			hands_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			last_cmd = cmd;
			sprintf(resp_buf, "last_cmd: %d, state: %d", last_cmd, state);
			break;
		case 13: // left up
			hands_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			last_cmd = cmd;
			sprintf(resp_buf, "last_cmd: %d, state: %d", last_cmd, state);
			break;
		case 14: // left down
			hands_limit = 0;
			if (0) // error state
			{
				state = 1;
			}
			last_cmd = cmd;
			sprintf(resp_buf, "last_cmd: %d, state: %d", last_cmd, state);
			break;
	}
}

void handsWork()
{
	if (last_cmd == 11 & (!hands_limit & hands_cnt < 1000))
	{
		hands_spin = 1;
		hands_cnt += hands_pwm;
		Serial.println(hands_cnt);
	} else {
		hands_limit = 1;
		hands_spin = 0;
	}
}