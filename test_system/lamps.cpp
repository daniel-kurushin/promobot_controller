#include "lamps.hpp"
#include "Arduino.h"
#include "pindefines.hpp"

volatile uint32_t lamps_cnt = 0;
volatile uint8_t lamps_cmd = 0;
volatile uint8_t lamps_state = 0;
volatile uint8_t lamps_state_cnt = 0;

void lamp_wht_on()
{
	LAMP_WHT_PORT &= ~_BV(LAMP_WHT_PIN); //relay module inverts
}

void lamp_wht_off()
{
	LAMP_WHT_PORT |= _BV(LAMP_WHT_PIN);
}

void lamp_red_on()
{
	LAMP_RED_PORT &= ~_BV(LAMP_RED_PIN);
}

void lamp_red_off()
{
	LAMP_RED_PORT |= _BV(LAMP_RED_PIN);
}

volatile void lampsFlash()
{
	if (lamps_state == 0)
	{
		lamps_state = 1;
	}
	switch (lamps_state)
	{
		case 1:
			// Serial.println(lamps_cnt);
			if (++lamps_cnt > TIMER_1_SECOND / 4)
			{
				lamp_wht_on();
				lamp_red_on();
				lamps_state = 2;
				lamps_cnt = 0;
			}
			break;
		case 2:
			// Serial.println(2);
			if (++lamps_cnt > TIMER_1_SECOND / 4)
			{
				lamp_wht_off();
				lamp_red_off();
				lamps_cnt = 0;
				if (++lamps_state_cnt > 3)
				{
					lamps_state = 3;
				} else
				{
					lamps_state = 1;
				}
			}
			break;
		case 3:
			// Serial.println(3);		
			lamp_wht_off();
			lamp_red_off();
			break;
	}
}

volatile void lampsWork()
{
	// Serial.
	switch (lamps_cmd) 
	{
		case 0:
			lamp_wht_off();
			lamp_red_off();
			break;
		case 1:
			lampsFlash();
			break;
	}
}