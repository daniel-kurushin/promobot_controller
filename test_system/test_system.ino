#include "pindefines.h"

#define COMP_SWITCHED_OFF 0x00
#define COMP_LOADED 0x01
#define COMP_ERROR 0x02
#define COMP_LOADING 0x03

#define MAX_BOOT_TIME 1000*10*60 //10 minutes
#define COMP_LOADED_SIGNAL 123456

uint16_t computer_state = COMP_SWITCHED_OFF;
int hands_state = 0x00;
uint32_t comp_boot_time = 0;

void init_comp_relays()
{
	COMP_PWR_REL_DDR |= _BV(COMP_PWR_REL_PIN);
	COMP_PWR_BTN_DDR |= _BV(COMP_PWR_BTN_PIN);
}

void comp_on()
{
	Serial.println('1');
	COMP_PWR_REL_PORT &= ~_BV(COMP_PWR_REL_PIN);
	delay(4000);
	COMP_PWR_BTN_PORT &= ~_BV(COMP_PWR_BTN_PIN);
	delay(100);
	COMP_PWR_BTN_PORT |= _BV(COMP_PWR_BTN_PIN);
}

void comp_off()
{
	Serial.println('2');
	COMP_PWR_REL_PORT |= _BV(COMP_PWR_REL_PIN);
}

void lamps_init()
{
	LAMP_WHT_DDR |= _BV(LAMP_WHT_PIN);
	LAMP_RED_DDR |= _BV(LAMP_RED_PIN);
}

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

void setup()
{
	Serial.begin(9600);
	init_comp_relays();
  	lamps_init();
}

void loop()
{
	switch (computer_state)
	{
		case COMP_SWITCHED_OFF:
			comp_on();
			comp_boot_time = millis();
			computer_state = COMP_LOADING;
			break;
		case COMP_LOADING:
			if (millis()-comp_boot_time > MAX_BOOT_TIME)
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

			break;
		case COMP_ERROR:
			show_comp_error();
			break;
	}
}
