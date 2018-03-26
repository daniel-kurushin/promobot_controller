
#include "computer.hpp"
#include "Arduino.h"
#include "pindefines.hpp"

#define COMP_SWITCHED_OFF 0x00
#define COMP_LOADED 0x01
#define COMP_ERROR 0x02
#define COMP_LOADING 0x03
#define STATE_IDLE 0x04
#define COMP_SWITCHING 0x05

#define MAX_BOOT_TIME 1000*10*60 //10 minutes
#define COMP_LOADED_SIGNAL 123456

#define CMD_STAT_ERR 0
#define CMD_STAT_OK 1


uint16_t computer_state = COMP_SWITCHED_OFF;
uint16_t hands_state = 0x00;
uint32_t comp_boot_time = 0;

void lamp_wht_on()
{
	Serial.println(3);
	LAMP_WHT_PORT &= ~_BV(LAMP_WHT_PIN);
}

void lamp_wht_off()
{
	Serial.println(4);
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

void show_comp_error()
{
	while(1)
	{
		lamp_wht_on();
		lamp_red_on();
		delay(1000);
		lamp_wht_off();
		lamp_red_off();
		delay(1000);
	}
}

void comp_on()
{
	COMP_PWR_REL_PORT &= ~_BV(COMP_PWR_REL_PIN);
	delay(4000);
	COMP_PWR_BTN_PORT &= ~_BV(COMP_PWR_BTN_PIN);
	delay(100);
	COMP_PWR_BTN_PORT |= _BV(COMP_PWR_BTN_PIN);
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
			Serial.println(1);
			comp_on();
			comp_boot_time = millis();
			computer_state = COMP_LOADING;
			break;
		case COMP_SWITCHING:
			// use counter here to wait after turning on power relay of computer
		case COMP_LOADING:
			Serial.println(2);
			if (millis() - comp_boot_time > MAX_BOOT_TIME)
			{
				// comp_off(); //temp
				computer_state = COMP_ERROR;
			}
			if (Serial.available() & Serial.parseInt() == COMP_LOADED_SIGNAL)
			{
				computer_state = COMP_LOADED;
			}
			break;
		case COMP_LOADED:
			computer_state = STATE_IDLE;
			break;
		case STATE_IDLE:
			// if (Serial.available())
			// {
			// 	cmd = Serial.parseInt();
			// }
			break;
		case COMP_ERROR:
			Serial.println(3);
			show_comp_error();
			break;
	}
}