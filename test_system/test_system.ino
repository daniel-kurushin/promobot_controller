#include "pindefines.hpp"
#include "Arduino.h"
#include "legs.hpp"
#include "hands.hpp"
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

void lamps_init()
{
	LAMP_WHT_DDR |= _BV(LAMP_WHT_PIN);
	LAMP_RED_DDR |= _BV(LAMP_RED_PIN);
}

void init_comp_relays()
{
	COMP_PWR_REL_DDR |= _BV(COMP_PWR_REL_PIN);
	COMP_PWR_BTN_DDR |= _BV(COMP_PWR_BTN_PIN);
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
  	ptme = 0;
	noInterrupts();           // disable all interrupts

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // 1024 prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2  = 0;

	OCR2A = 5;            // compare match register 16MHz/256/2Hz
	TCCR2B |= (1 << WGM21);   // CTC mode
	TCCR2B |= (1 << CS20);    // no prescaling
	TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt

	DDRA |= _BV(M1AIN) | _BV(M1BIN) | _BV(M1PWM);
	DDRB |= _BV(LFTSTSWPIN) | _BV(RGTSTSWPIN) | _BV(TRIG_1_PIN);
	DDRB &= ~_BV(ECHO_1_PIN);
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
	// Serial.print(sonar_data[0]);
	// Serial.println(sonar_data[1]);
	// delay(100);
	computerWork();
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
		Serial.println(resp_buf);
	} else {
		legsWork();
		handsWork();
	}
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	// Serial.println("ISR1");
	// handsWork();
	// legsWork();
}

ISR(TIMER2_COMPA_vect)
{
	TCNT2 = 0;
  	time++;

    sonarWork();
    // sonar_results[sonar_data[0]]->accuracy = sonar_data[2];
    // sonar_results[sonar_data[0]]->distance = sonar_data[1];
	// handsWork();
	// legsWork();
}

