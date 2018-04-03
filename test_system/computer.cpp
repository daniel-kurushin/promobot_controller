
#include "computer.hpp"
#include "Arduino.h"
#include "pindefines.hpp"

#define COMP_SWITCHED_OFF 0x00
#define COMP_LOADED 0x01
#define COMP_ERROR_BLINK_ON 0x02
#define COMP_ERROR_BLINK_OFF 0x03
#define COMP_LOADING 0x04
#define STATE_IDLE 0x05
#define COMP_SWITCHING_ON 0x06

#define MAX_BOOT_TIME 1000*10*2 //10 minutes
#define COMP_LOADED_SIGNAL 123456

#define CMD_STAT_ERR 0
#define CMD_STAT_OK 1


uint16_t computer_state = COMP_SWITCHED_OFF;
uint16_t hands_state = 0x00;
uint32_t comp_boot_time = 0;
uint32_t comp_error_blink_time = 0;
volatile uint8_t lamps_cnt = 0;
volatile uint8_t lamps_state = 0;
volatile uint8_t lamps_state_cnt = 0;

void lamp_wht_on()
{
	LAMP_WHT_PORT &= ~_BV(LAMP_WHT_PIN);
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

void comp_off()
{
	COMP_PWR_REL_PORT |= _BV(COMP_PWR_REL_PIN);
}

void computerWork()
{
	switch (computer_state)
	{	
		case COMP_SWITCHED_OFF:
			// comp_on();
			COMP_PWR_REL_PORT &= ~_BV(COMP_PWR_REL_PIN); //turn on pwr comp relay 
			comp_boot_time = millis();
			computer_state = COMP_SWITCHING_ON;
			Serial.println(1);
			break;
		case COMP_SWITCHING_ON:
			Serial.println(2);
			if (millis() - comp_boot_time > 4000 & millis() - comp_boot_time < 4100)
			{
				Serial.println(3);
				COMP_PWR_BTN_PORT &= ~_BV(COMP_PWR_BTN_PIN);
			} else if (millis() - comp_boot_time > 4100)
			{
				Serial.println(4);
				COMP_PWR_BTN_PORT |= _BV(COMP_PWR_BTN_PIN);
				computer_state = COMP_LOADING;
			}
			// use counter here to wait after turning on power relay of computer
		case COMP_LOADING:
			if (millis() - comp_boot_time > MAX_BOOT_TIME)
			{
				// comp_off(); //temp
				computer_state = COMP_ERROR_BLINK_ON;
			}
			if (Serial.available() & Serial.parseInt() == COMP_LOADED_SIGNAL)
			{
				computer_state = COMP_LOADED;
				Serial.println(COMP_LOADED_SIGNAL);
			}
			break;
		case COMP_LOADED:
			// computer_state = STATE_IDLE;
			// TODO: move this switch to standalone function
			if (lamps_state == 0)
			{
				lamps_state = 1;
			}
			switch (lamps_state)
			{
				case 1:
					if (++lamps_cnt > 100)
					{
						lamp_wht_on();
						lamp_red_on();
						lamps_state = 2;
					}
					break;
				case 2:
					if (++lamps_cnt > 100)
					{
						lamp_wht_off();
						lamp_red_off();
						if (++lamps_state_cnt > 3)
						{
							lamps_state = 3;
						}
						lamps_state = 1;
					}
					break;
				case 3:
					lamp_wht_off();
					lamp_red_off();
					break;
			}
			break;
		case STATE_IDLE:
			// if (Serial.available())
			// {
			// 	cmd = Serial.parseInt();
			// }
			break;
		case COMP_ERROR_BLINK_ON:
			comp_error_blink_time = millis();
			if (millis() - comp_error_blink_time > 500 & millis() - comp_error_blink_time < 1000)
			{
				lamp_wht_on();
				lamp_red_on();
				computer_state = COMP_ERROR_BLINK_OFF;
			}
			break;
		case COMP_ERROR_BLINK_OFF:
			comp_error_blink_time = millis();
			if (millis() - comp_error_blink_time > 500 & millis() - comp_error_blink_time < 1000)
			{
				lamp_wht_off();
				lamp_red_off();
				computer_state = COMP_ERROR_BLINK_ON;
			}
			break;
	}
}