#include "palms.hpp"
#include "Arduino.h"
#include "pindefines.hpp"

volatile uint32_t palms_cnt = 0;
volatile uint8_t palms_cmd = 0;
volatile uint8_t lft_palm_state = 0;
volatile uint8_t rgt_palm_state = 0;

void palmsWork()
{
	lftPalmWork();
	rgtPalmWork();
}

void lftPalmWork()
{
	switch (lft_palm_state)
	{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
	}
}

void rgtPalmWork()
{
	switch (rgt_palm_state)
	{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
	}
}

void processPalms(char *resp_buf, int palms_cmd)
{
	switch (palms_cmd) 
	{
		case 40:
			break;
		case 41:
			break;
		case 42:
			break;
		case 43:
			break;
	}
}