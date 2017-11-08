#include <IRremoteInt.h>
#include <IRremote.h>
#include <Wire.h>
#include <GY_85.h>
#include <math.h>
#define IR_USE_TIMER2

#define M1AIN PA0 // 22 -> 1 (IN A on scheme)
#define M1BIN PA1 // 23 -> 6
#define M1PWM PA2 // 24 -> 5
// #define M1AEN PA3 // 25 -> 2
// #define M1BEN PA4 // 26 -> 3 (IN B on scheme)

#define LFTSTSWPIN PB0
#define RGTSTSWPIN PB1

#define LH_NOT_LIMIT (PINB & _BV(LFTSTSWPIN)) == 1
#define LH_LIMIT !LH_NOT_LIMIT
#define RH_NOT_LIMIT (PINB & _BV(RGTSTSWPIN)) == 1
#define RH_LIMIT !RH_NOT_LIMIT

#define M2AIN PC4 // 33 -> 1
#define M2BIN PC3 // 34 -> 6
#define M2PWM PC2 // 35 -> 5
// #define M2AEN PC1 // 36 -> 2
// #define M2BEN PC0 // 37 -> 3

#define S0TRI PL0
#define S1TRI PL1
#define S2TRI PL2
#define S3TRI PL3

#define AIN PE5
#define VIN PG5
#define MOSFET PE3

//#define RH_GO_UP PORTL |= _BV(RHUP)
//#define RH_GO_DOWN PORTL |= _BV(RHDOWN)
#define LH_STOP PORTG &= ~_BV(LEFTHAND)
#define RH_STOP PORTG &= ~_BV(RIGHTHAND)

#define RIGHTHAND PG1 // 40
#define LEFTHAND PG0 // 41
#define RHUP PL4 // 45
#define LHUP PL5 // 44
#define RHDOWN PL6 // 43
#define LHDOWN PL7 // 42

#define SECHO PB0

#define LEFT  16716015
#define RIGHT 16734885
#define FWD   16718055
#define BACK  16730805
#define STOP  16726215
#define RIGHTHANDUP 16761405
#define RIGHTHANDDOWN 16748655
#define RIGHTHANDSTOP 16756815
#define LEFTHANDUP 16720605
#define LEFTHANDDOWN 16769055
#define LEFTHANDSTOP 16738455
#define NOCMD 0x00

#define LH_IS_NEW_CMD (LH_Command != old_LH_Command)
#define RH_IS_NEW_CMD (RH_Command != old_RH_Command)

#define HANDS_PWM_UP 254
#define HANDS_PWM_DN 100

GY_85 GY85;

float gz = 0;
float pure_gz = 0;
float z = 0;
float dt = 0.01;
float delta = 0.0;
float old_z = 0;
float new_z = 0;
float Kp = 10;
uint16_t tme = 0;
uint8_t lft_pwm = 0;
uint8_t rgt_pwm = 0;
unsigned long cmd = 0;
int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);
decode_results results;

float trg_spd = 0;

typedef enum HAND_{
	HAND_STATE_INIT,
	HAND_STATE_GO_DOWN,
	HAND_STATE_SPIN_UP,
	HAND_STATE_SPIN_DOWN,
	HAND_STATE_BEGIN_SPIN_UP,
	HAND_STATE_BEGIN_SPIN_DOWN,
	HAND_STATE_STOP,
	HAND_STATE_SPIN_FAIL,
	HAND_STATE_IDLE
} State_t;

volatile State_t LH_State, RH_State = HAND_STATE_INIT;
volatile uint32_t LH_Command, old_LH_Command, RH_Command, old_RH_Command = 0;
volatile uint16_t LH_Time, RH_Time = 0;
volatile State_t old_LH_State, old_RH_State = NULL;
volatile uint8_t hands_pwm = 0;
volatile uint8_t adc_val = 0;

// DDRF |= (1 << M1PWM);
// DDRK |= (1 << M2PWM);

// Set Timer 0 prescaler to clock/8.
// At 9.6 MHz this is 1.2 MHz.
// See ATtiny13 datasheet, Table 11.9.
// TCCR0B |= (1 << CS01);

// // Set to 'Fast PWM' mode
// // Enable 'Fast PWM'
// TCCR0A |= (1 << WGM01) | (1 << WGM00);

// // Clear OC0B and OC0A(!!!) output on compare match, upwards counting.
// TCCR0A |= (1 << COM0A0) | (1 << COM0B0);

void measure_v()
{
	
}

void measure_a()
{

}

void charge()
{

}

void lft_stop()
{
	lft_pwm = 0;
}

void rgt_stop()
{
	rgt_pwm = 0;
}

void lft_frw(float spd) //TODO ; m/s!!!
{
	PORTA |= _BV(M1AIN);
	PORTA &= ~_BV(M1BIN);

	lft_pwm = (int) max(min(spd,255),0);
	// OCR0A = lft_pwm;
}

void rgt_frw(float spd)
{
	PORTC |= _BV(M2AIN);
	PORTC &= ~_BV(M2BIN);

	rgt_pwm = (int) max(min(spd,255),0);
	// analogWrite(M2PWM, rgt_pwm);
}

void process_IR_cmd(unsigned long cmd)
{
	switch (cmd)
	{
		case LEFT:
			z -= 10;
			break;
		case RIGHT:
			z += 10;
			break;
		case FWD:
			trg_spd += 10;
			break;
		case BACK:
			trg_spd -= 10;
			break;
		case STOP:
			trg_spd = 0;
			z = 0;
			break;
		case LEFTHANDUP:
			LH_Command = LEFTHANDUP;
			break;
		case LEFTHANDDOWN:
			LH_Command = LEFTHANDDOWN;
			break;
		case LEFTHANDSTOP:
			RH_Command = LEFTHANDSTOP;
			break;
		case RIGHTHANDUP:
			RH_Command = RIGHTHANDUP;
			break;
		case RIGHTHANDDOWN:
			RH_Command = RIGHTHANDDOWN;
			break;
		case RIGHTHANDSTOP:
			RH_Command = RIGHTHANDSTOP;
			break;
	}
	z = min(max(-180, z), 180);
	trg_spd = min(max(0, trg_spd), 255);
}

void setup()
{
	Wire.begin();
	Serial.begin(9600);
	GY85.init();
	irrecv.enableIRIn();

	old_z = GY85.gyro_z(GY85.readGyro());
	for (int i=0; i < 10; i++)
	{
		delay(300);
		new_z = GY85.gyro_z(GY85.readGyro());
		delta += new_z - old_z;
	}
	delta /= 10;

	noInterrupts();           // disable all interrupts

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	// OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // 1024 prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

	DDRA |= _BV(M1AIN) | _BV(M1BIN) | _BV(M1PWM);
	DDRB |= _BV(LFTSTSWPIN) | _BV(RGTSTSWPIN);
	DDRC |= _BV(M2AIN) | _BV(M2BIN) | _BV(M2PWM);
	DDRL |= _BV(S0TRI) | _BV(S1TRI) | _BV(S2TRI) | _BV(S3TRI) | _BV(RHUP) | _BV(LHUP) | _BV(RHDOWN) | _BV(LHDOWN);
	DDRG |= _BV(RIGHTHAND) | _BV(LEFTHAND);

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


void LH_GO_UP() {	PORTL |= _BV(LHUP); PORTL &= ~_BV(LHDOWN); PORTG |= _BV(LEFTHAND); }
void LH_GO_DOWN() {  PORTL |= _BV(LHDOWN); PORTL &= ~_BV(LHUP); PORTG |= _BV(LEFTHAND); }

void RH_GO_UP() {	PORTL |= _BV(RHUP); PORTL &= ~_BV(RHDOWN); PORTG |= _BV(RIGHTHAND); }
void RH_GO_DOWN() {  PORTL |= _BV(RHDOWN); PORTL &= ~_BV(RHUP); PORTG |= _BV(RIGHTHAND); }

unsigned long readIRC()
{
	unsigned long res = NOCMD;
	// digitalWrite(13,1);
	// read ir reciever
	if (irrecv.decode(&results)) {
		res = results.value;
		irrecv.resume();
	}
	return res;
}

void rigthHandWork()
{
	switch (RH_State)
	{
		case HAND_STATE_INIT:
			RH_Time = 0;
			RH_State = HAND_STATE_GO_DOWN;
		break;
			case HAND_STATE_GO_DOWN:
			old_RH_Command = RIGHTHANDDOWN;
			RH_GO_DOWN();
			if (RH_LIMIT || (++RH_Time > 1000)) RH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_DOWN:
			RH_GO_DOWN();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (!RH_LIMIT & (++RH_Time > 10)) RH_State = HAND_STATE_SPIN_DOWN;
			break;
		case HAND_STATE_SPIN_DOWN:
			RH_GO_DOWN();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (RH_LIMIT) RH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_UP:
			RH_GO_UP();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (!RH_LIMIT & (++RH_Time > 10)) RH_State = HAND_STATE_SPIN_UP;
			break;
		case HAND_STATE_SPIN_UP:
			RH_GO_UP();
			if (++RH_Time > 10000) RH_State = HAND_STATE_SPIN_FAIL;
			if (RH_LIMIT) RH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_STOP:
			RH_STOP;
			RH_Time = 0;
			RH_State = HAND_STATE_IDLE;
			break;
		case HAND_STATE_SPIN_FAIL:
			RH_STOP;
			RH_Time = 0;
			break;
		case HAND_STATE_IDLE:
			if ((RH_IS_NEW_CMD) & (RH_Command == RIGHTHANDUP))
			{
				RH_State = HAND_STATE_BEGIN_SPIN_UP;
				RH_Time = 0;
				old_RH_Command = RH_Command;
			}
			if ((RH_IS_NEW_CMD) & (RH_Command == RIGHTHANDDOWN))
			{
				RH_State = HAND_STATE_BEGIN_SPIN_DOWN;
				RH_Time = 0;
				old_RH_Command = RH_Command;
			}
			break;
	}
	old_RH_State = RH_State;
}


void leftHandWork()
{
	switch (LH_State)
	{
		case HAND_STATE_INIT:
			hands_pwm = 0;
			LH_Time = 0;
			LH_State = HAND_STATE_GO_DOWN;
		break;
			case HAND_STATE_GO_DOWN:
			hands_pwm = HANDS_PWM_DN;
			old_LH_Command = LEFTHANDDOWN;
			LH_GO_DOWN();
			if (LH_LIMIT || (++LH_Time > 1000)) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_DOWN:
			hands_pwm = HANDS_PWM_DN;
			LH_GO_DOWN();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_NOT_LIMIT & (++LH_Time > 10)) LH_State = HAND_STATE_SPIN_DOWN;
			break;
		case HAND_STATE_SPIN_DOWN:
			hands_pwm = HANDS_PWM_DN;
			LH_GO_DOWN();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_LIMIT) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_BEGIN_SPIN_UP:
			hands_pwm = HANDS_PWM_UP;
			LH_GO_UP();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_NOT_LIMIT & (++LH_Time > 10)) LH_State = HAND_STATE_SPIN_UP;
			break;
		case HAND_STATE_SPIN_UP:
			hands_pwm = HANDS_PWM_UP;
			LH_GO_UP();
			if (++LH_Time > 10000) LH_State = HAND_STATE_SPIN_FAIL;
			if (LH_LIMIT) LH_State = HAND_STATE_STOP;
			break;
		case HAND_STATE_STOP:
			hands_pwm = 0;
			LH_STOP;
			LH_Time = 0;
			LH_State = HAND_STATE_IDLE;
			break;
		case HAND_STATE_SPIN_FAIL:
			hands_pwm = 0;
			LH_STOP;
			LH_Time = 0;
			break;
		case HAND_STATE_IDLE:
			if ((LH_IS_NEW_CMD) & (LH_Command == LEFTHANDUP))
			{
				LH_State = HAND_STATE_BEGIN_SPIN_UP;
				LH_Time = 0;
				old_LH_Command = LH_Command;
			}
			if ((LH_IS_NEW_CMD) & (LH_Command == LEFTHANDDOWN))
			{
				LH_State = HAND_STATE_BEGIN_SPIN_DOWN;
				LH_Time = 0;
				old_LH_Command = LH_Command;
			}
			break;
	}
	old_LH_State = LH_State;
}


void loop()
{
	pure_gz = GY85.gyro_z(GY85.readGyro());
	gz += (pure_gz - delta) * dt;
	// Serial.println("1");
	cmd = readIRC();
	// Serial.println("2");
	if (cmd != NOCMD)
	{
		process_IR_cmd(cmd);
		// Serial.println("3");
	}

	lft_frw(trg_spd + Kp * atan(z - gz));
	rgt_frw(trg_spd - Kp * atan(z - gz));
	// Serial.println("4");
	if(++tme > 100)
	{
		tme = 0;
		// Serial.print("\tpure_gz "); Serial.print(pure_gz);
		// Serial.print("\tgz ");      Serial.print(gz);
		// Serial.print("\tlft_pwm "); Serial.print(lft_pwm);
		// Serial.print("\trgt_pwm "); Serial.print(rgt_pwm);
		// Serial.print("\ttrg_spd "); Serial.print(trg_spd);
		// Serial.print("\tz "); 		Serial.print(z);
		Serial.print("\trh_state "); 		Serial.print(RH_State);
		Serial.print("\trh_cmd "); 		Serial.print(RH_Command);
		Serial.print("\trh_old_cmd "); 		Serial.print(RH_Command);
		Serial.print("\trh_not_limit "); 		Serial.print(RH_NOT_LIMIT);
		// (LH_Command != old_LH_Command) & !LH_NOT_LIMIT
		Serial.print("\trh_is_new_cmd "); 		Serial.print(RH_IS_NEW_CMD);
		Serial.print("\tadc_val "); 		Serial.print(adc_val);

		Serial.print("\tcmd ");     Serial.print(cmd);
		Serial.println();
	}

	delay(dt * 1000);
}

volatile uint8_t legs_cnt = 0;
volatile uint8_t hands_cnt = 0;
volatile uint8_t m1cnt = 0;
volatile uint8_t m2cnt = 0;
volatile uint8_t mhcnt = 0;

void handsWork() 
{
	if (hands_cnt++ == 0)
	{
		leftHandWork();
		rigthHandWork();
		mhcnt = 0;
	}
	if (mhcnt++ > hands_pwm)
	{
		LH_STOP;
		RH_STOP;
	}
}

void legsWork()
{
	if (legs_cnt++ == 0)
	{
		PORTA |= _BV(M1PWM);
		PORTC |= _BV(M2PWM);
		m1cnt = m2cnt = 0;
	}
	if (m1cnt++ > lft_pwm)
	{
		PORTA &= ~_BV(M1PWM);
	}
	if (m2cnt++ > rgt_pwm)
	{
		PORTC &= ~_BV(M2PWM);
	}
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	handsWork();
	// legsWork();
}

ISR(ADC_vect) {
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA|=(1<<ADIF);
	adc_val = ADCH;
}


