
#include "computer.hpp"
#include "Arduino.h"
#include "pindefines.hpp"
#include "lamps.hpp"

#define COMP_SWITCHED_OFF 0x00
#define COMP_SWITCHING_ON 0x01
#define COMP_LOADING 0x02
#define COMP_LOADED 0x03
// #define COMP_ERROR_BLINK_ON 0x02
// #define COMP_ERROR_BLINK_OFF 0x03
#define STATE_IDLE 0x10
#define COMP_WAIT_FOR_PWR TIMER_1_SECOND * 6

#define MAX_BOOT_TIME 1000*10*20 //10 minutes
#define COMP_LOADED_SIGNAL 51

#define CMD_STAT_ERR 0
#define CMD_STAT_OK 1


volatile uint16_t computer_state = COMP_SWITCHED_OFF;
volatile uint32_t comp_boot_time = 0;
uint32_t comp_error_blink_time = 0;
volatile uint8_t comp_last_cmd = 0;
void comp_off()
{
	COMP_PWR_REL_PORT |= _BV(COMP_PWR_REL_PIN);
}

void processComp(char *resp_buf, uint8_t cmd)
{
	switch (cmd)
	{
		case 50: // get state
			comp_last_cmd = cmd;
			break;
		case COMP_LOADED_SIGNAL:
			// comp loaded
			computer_state = COMP_LOADED;
			break;
		case 52:
			break;
	}
}

void computerWork()
{
	// if (comp_boot_time % 100 == 0) Serial.println(comp_boot_time);
	// if (comp_boot_time % 10 == 0) Serial.println(computer_state);
	// Serial.println(computer_state);
	switch (computer_state)
	{	
		case COMP_SWITCHED_OFF:
			COMP_PWR_REL_PORT &= ~_BV(COMP_PWR_REL_PIN); //turn on pwr comp relay 
			comp_boot_time = 0;
			computer_state = COMP_SWITCHING_ON;
			break;
		case COMP_SWITCHING_ON:
			if (comp_boot_time > COMP_WAIT_FOR_PWR & comp_boot_time <= COMP_WAIT_FOR_PWR + TIMER_1_SECOND / 4)
			{
				COMP_PWR_BTN_PORT &= ~_BV(COMP_PWR_BTN_PIN);
			} else if (comp_boot_time > COMP_WAIT_FOR_PWR + TIMER_1_SECOND / 4)
			{
				COMP_PWR_BTN_PORT |= _BV(COMP_PWR_BTN_PIN);
				computer_state = COMP_LOADING;
			}
			comp_boot_time++;
			// use counter here to wait after turning on power relay of computer
		case COMP_LOADING:
			if (comp_boot_time < MAX_BOOT_TIME)
			{
				comp_boot_time++;
				// here should be cmd from computer recieved in main loop
				// if (comp_boot_time > 72000) computer_state = COMP_LOADED; 
				// comp_off(); //temp
				// computer_state = COMP_ERROR_BLINK_ON;
			} else {
				//error
			}
			break;
		case COMP_LOADED:
			// computer_state = STATE_IDLE;
			// LAMP_WHT_PORT &= ~_BV(LAMP_WHT_PIN);
			// lamps_work();
			// Serial.println(12);
			lamps_cmd = 1;
			// other staff
			break;
		case STATE_IDLE:
			// if (Serial.available())
			// {
			// 	cmd = Serial.parseInt();
			// }
			break;
		// case COMP_ERROR_BLINK_ON:
		// 	break;
		// case COMP_ERROR_BLINK_OFF:
		// 	break;
	}
}