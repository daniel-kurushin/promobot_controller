// main

#include "pindefines.hpp"
#include "Arduino.h"
#include "legs.hpp"
#include "lamps.hpp"
#include "hands.hpp"
#include "palms.hpp"
#include "computer.hpp"
#include "d_sensors.hpp"
#include <Wire.h>
#include <GY_85.h>
#include <math.h>

char cz[10] = "";

char resp_buf[160];
uint16_t cmd = 0;
uint8_t sys = 0;

volatile uint16_t sonar_data[SONAR_DATA_LEN];

// init relays
void lamps_init()
{
	LAMP_WHT_DDR |= _BV(LAMP_WHT_PIN);
	LAMP_RED_DDR |= _BV(LAMP_RED_PIN);
}

void init_comp_relays()
{
	COMP_PWR_REL_DDR |= _BV(COMP_PWR_REL_PIN);
	COMP_PWR_BTN_DDR |= _BV(COMP_PWR_BTN_PIN);
	COMP_PWR_REL_PORT |= _BV(COMP_PWR_REL_PIN); // turn off comp pwr relay
}

void init_motors_relays()
{
	MOTORS_REL_PWR_DDR |= _BV(MOTORS_REL_PWR_PIN);
}

void motors_relays_on()
{
	MOTORS_REL_PWR_PORT &= ~_BV(MOTORS_REL_PWR_PIN);
}

void init_5v_relays()
{
	PWR_5V_DDR |= _BV(PWR_5V_PIN);
}

void bus_5v_on()
{
	PWR_5V_PORT &= ~_BV(PWR_5V_PIN);
}

// rs485
void rs485_trsm_en()
{
  PORTF |= _BV(RS485_TRSM);
  PORTF &= ~_BV(RS485_RECV);
}

void rs485_recv_en()
{
  PORTF &= ~_BV(RS485_TRSM);
  PORTF |= _BV(RS485_RECV);
}

void setup()
{
	Serial.begin(9600);
	Serial.setTimeout(5);
	init_comp_relays();
  	lamps_init();
  	init_motors_relays();
	motors_relays_on();
	init_5v_relays();
	bus_5v_on();

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
  	ptme = 0;
	noInterrupts();           // disable all interrupts

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // no prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2  = 0;

	OCR2A = 5;            // compare match register 16MHz/256/2Hz
	TCCR2B |= (1 << WGM21);   // CTC mode
	TCCR2B |= (1 << CS20);    // no prescaling
	TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt

	DDRA |= _BV(M1AIN) | _BV(M1BIN) | _BV(M1PWM);
	DDRB |= _BV(TRIG_1_PIN);
	DDRH &= ~_BV(LFTSTSWPIN);
	DDRH &= ~_BV(RGTSTSWPIN);
	DDRB &= ~_BV(ECHO_1_PIN);
	DDRC |= _BV(M2AIN) | _BV(M2BIN) | _BV(M2PWM);
	DDRF |= _BV(RS485_TRSM);
	DDRF |= _BV(RS485_RECV);
	// DDRL |= _BV(S0TRI) | _BV(S1TRI) | _BV(S2TRI) | _BV(S3TRI);
	DDRL |= _BV(RHUP) | _BV(LHUP) | _BV(RHDOWN) | _BV(LHDOWN);
	DDRG |= _BV(RIGHTHAND) | _BV(LEFTHAND);

	COMP_PWR_BTN_PORT |= _BV(COMP_PWR_BTN_PIN); // turn off comp btn relay

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
	// Serial.print(sonar_data[0]);
	// Serial.println(sonar_data[1]);
	// delay(100);
	// rs485_recv_en();
	if (Serial.available())
	{	
		cmd = Serial.parseInt();
		sys = cmd / 10;
		Serial.println("Sys:");
		Serial.println(sys);
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
			case 4:
				processPalms(resp_buf, cmd);
				break;
			case 5:
				processComp(resp_buf, cmd);
				break;
			// case 6:
			// 	processRedLamps(cmd);
			// 	break;
			// case 7:
			// 	processWhiteLamps(cmd);
			// 	break;
			// case 8:
			// 	processMotionSensor(cmd);
			// 	break;
			case 9:
				processBottomDSs(resp_buf, cmd);
				break;
			// case 10:
			// 	processTopDS(cmd);
			// 	break;
			// case 11:
			// 	processGyro(cmd);
			// 	break;
		}
		// rs485_trsm_en();
		Serial.println(resp_buf);
	} else {
		legsWork();
		// handsWork();
	}
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;

	computerWork();
	lampsWork();
	handsWork();
}

ISR(TIMER2_COMPA_vect)
{
	TCNT2 = 0;
  	time++;

    sonarWork();
}

