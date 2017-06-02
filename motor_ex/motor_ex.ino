#include <Wire.h>
#include <GY_85.h>
#include <math.h>
#include <IRremote.h>
#define IR_USE_TIMER2

#define M1AIN PA0
#define M1BIN PA1
#define M1PWM PA2
#define M1AEN PA3
#define M1BEN PA4

#define M2AIN PC4
#define M2BIN PC3
#define M2PWM PC2
#define M2AEN PC1
#define M2BEN PC0

#define S0TRI PL0
#define S1TRI PL1
#define S2TRI PL2
#define S3TRI PL3

#define SECHO PB0

#define LEFT  16716015
#define RIGHT 16734885
#define FWD   16718055
#define BACK  16730805
#define STOP  16726215
#define NOCMD 0x00

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

void lft_stop()
{
	digitalWrite(M1AEN, LOW);
	digitalWrite(M1BEN, LOW);
}

void rgt_stop()
{
	digitalWrite(M2AEN, LOW);
	digitalWrite(M2BEN, LOW);
}

void lft_frw(float spd) //TODO ; m/s!!!
{
	PORTA |= _BV(M1AEN) | _BV(M1AIN);
	PORTA &= ~_BV(M1BIN);

	lft_pwm = (int) max(min(spd,255),0);
	// OCR0A = lft_pwm;
}

void rgt_frw(float spd)
{
	PORTC |= _BV(M2AEN) | _BV(M2AIN);
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

	noInterrupts();           // disable all interrupts

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	// OCR1A = 3125;            // compare match register 16MHz/256/2Hz
	TCCR1B |= (1 << WGM12);   // CTC mode
	TCCR1B |= (1 << CS10);    // 1024 prescaler
	TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

	DDRA |= _BV(M1AIN) | _BV(M1BIN) | _BV(M1PWM) | _BV(M1AEN) | _BV(M1BEN);
	DDRC |= _BV(M2AIN) | _BV(M2BIN) | _BV(M2PWM) | _BV(M2AEN) | _BV(M2BEN);

	interrupts();             // enable all interrupts

	old_z = GY85.gyro_z(GY85.readGyro());
	for (int i=0; i < 10; i++)
	{
		delay(300);
		new_z = GY85.gyro_z(GY85.readGyro());
		delta += new_z - old_z;
	}
	delta /= 10;
}

unsigned long readIRC()
{
	unsigned long res = NOCMD;
	digitalWrite(13,1);
	// read ir reciever
	if (irrecv.decode(&results)) {
		res = results.value;
		irrecv.resume();
	}
	return res;
}


void loop()
{
	pure_gz = GY85.gyro_z(GY85.readGyro());
	gz += (pure_gz - delta) * dt;
	cmd = readIRC();
	if (cmd != NOCMD)
	{
		process_IR_cmd(cmd);
	}

	lft_frw(trg_spd - Kp * atan(z - gz));
	rgt_frw(trg_spd + Kp * atan(z - gz));

	if(++tme > 100)
	{
		tme = 0;
		Serial.print("\tpure_gz "); Serial.print(pure_gz);
		Serial.print("\tgz ");      Serial.print(gz);
		Serial.print("\tlft_pwm "); Serial.print(lft_pwm);
		Serial.print("\trgt_pwm "); Serial.print(rgt_pwm);
		Serial.print("\ttrg_spd "); Serial.print(trg_spd);
		Serial.print("\tz "); 		Serial.print(z);
		Serial.print("\tcmd ");     Serial.print(cmd);
		Serial.println();
	}

	delay(dt * 1000);
}

volatile uint8_t cnt = 0;
volatile uint8_t m1cnt = 0;
volatile uint8_t m2cnt = 0;

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	if (cnt++ == 0)
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

ISR(TIMER2_COMP_vect) {
  
}
