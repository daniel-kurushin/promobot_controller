#include "pindefines.hpp"
#include "Arduino.h"
#include "legs.hpp"
#include "hands.hpp"
#include <Wire.h>
#include <GY_85.h>
#include <math.h>

// String serialResponse = "";
char cz[10] = "";

#define COMP_SWITCHED_OFF 0x00
#define COMP_LOADED 0x01
#define COMP_ERROR 0x02
#define COMP_LOADING 0x03
#define STATE_IDLE 0

#define MAX_BOOT_TIME 1000*10*60 //10 minutes
#define COMP_LOADED_SIGNAL 123456

#define CMD_STAT_ERR 0
#define CMD_STAT_OK 1

/**/
uint32_t sys_state = 0;

uint16_t computer_state = COMP_SWITCHED_OFF;
uint16_t hands_state = 0x00;
uint32_t comp_boot_time = 0;
uint8_t sys = 0;
uint16_t cmd = 0;
char resp_buf[160];

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
	Serial.setTimeout(5);
	init_comp_relays();
  	lamps_init();
  	/* moved from motor_ex.ino */
  	Wire.begin();
	// GY85.init();
	// irrecv.enableIRIn();

	// old_z = GY85.gyro_z(GY85.readGyro());
	// for (int i=0; i < 10; i++)
	// {
		// delay(300);
		// new_z = GY85.gyro_z(GY85.readGyro());
		// delta += new_z - old_z;
	// }
	// delta /= 10;

	noInterrupts();           // disable all interrupts

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // 1024 prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

	DDRA |= _BV(M1AIN) | _BV(M1BIN) | _BV(M1PWM);
	DDRB |= _BV(LFTSTSWPIN) | _BV(RGTSTSWPIN);
	DDRC |= _BV(M2AIN) | _BV(M2BIN) | _BV(M2PWM);
	// DDRL |= _BV(S0TRI) | _BV(S1TRI) | _BV(S2TRI) | _BV(S3TRI) | _BV(RHUP) | _BV(LHUP) | _BV(RHDOWN) | _BV(LHDOWN);
	// DDRG |= _BV(RIGHTHAND) | _BV(LEFTHAND);

	//ADC setup
	ADCSRA |= (1 << ADPS2)
         | (1 << ADPS1)
         | (1 << ADPS0); // Set ADC prescaler to 128 - 125KHz sample rate @ 16MHz
  	// ADMUX |= (1 << REFS0); // Set ADC reference to AVCC

    ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	ADCSRA |= (1 << ADATE);
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADIE);  // Enable ADC Interrupt

	ADMUX &= ~(1 << MUX0)
	    &  ~(1 << MUX1)
	    &  ~(1 << MUX2)
	    &  ~(1 << MUX3);

	interrupts();             // enable all interrupts
}

void loop()
{
	if (Serial.available())
	{	
		cmd = Serial.parseInt();
		sys = cmd / 10;
		switch(sys)
		{
			case 1:
				processHands(resp_buf, cmd);
				break;
			case 2:
				processLegs(resp_buf, cmd);
				break;
			// case 3:
			// 	processHead(cmd);
			// 	break;
			// case 4:
			// 	processPalms(cmd);
			// 	break;
			// case 5:
			// 	processComp(cmd);
			// 	break;
			// case 6:
			// 	processRedLamps(cmd);
			// 	break;
			// case 7:
			// 	processWhiteLamps(cmd);
			// 	break;
			// case 8:
			// 	processMotionSensor(cmd);
			// 	break;
			// case 9:
			// 	processBottomDSs(cmd);
			// 	break;
			// case 10:
			// 	processTopDS(cmd);
			// 	break;
			// case 11:
			// 	processGyro(cmd);
			// 	break;
		}
		Serial.println(resp_buf);
	} else {
		legsWork();
		handsWork();
	}
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	// handsWork();
	// legsWork();
}

	// switch (computer_state)
	// {
	// 	case COMP_SWITCHED_OFF:
	// 		comp_on();
	// 		comp_boot_time = millis();
	// 		computer_state = COMP_LOADING;
	// 		break;
	// 	case COMP_LOADING:
	// 		if (millis() - comp_boot_time > MAX_BOOT_TIME)
	// 		{
	// 			// comp_off(); //temp
	// 			computer_state = COMP_ERROR;
	// 		}
	// 		if (Serial.available() & Serial.parseInt() == COMP_LOADED_SIGNAL)
	// 		{
	// 			computer_state = COMP_LOADED;
	// 		}
	// 		break;
	// 	case COMP_LOADED:
	// 		computer_state = STATE_IDLE;
	// 		break;
	// 	case STATE_IDLE:
	// 		if (Serial.available())
	// 		{
	// 			cmd = Serial.parseInt();
	// 		}
	// 		break;
	// 	case COMP_ERROR:
	// 		show_comp_error();
	// 		break;
	// }