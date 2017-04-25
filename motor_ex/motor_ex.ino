#include <Wire.h>
#include <GY_85.h>
#include <math.h>

#define M1AIN 7
#define M1BIN 6
#define M1PWM 5
#define M1AEN 4
#define M1BEN 3

#define M2AIN 8
#define M2BIN 9
#define M2PWM 10
#define M2AEN 11
#define M2BEN 12

#define READY 34
#define SYNC  33

#define LEFT  5
#define RIGHT 4
#define FWD   3
#define BACK  2
#define STOP  1
#define NOCMD 0

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
uint8_t cmd = 0;

float trg_spd = 0;

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
	// digitalWrite(M1AEN, HIGH);
	// digitalWrite(M1AIN, HIGH);
	// digitalWrite(M1BIN, LOW);
	//
	// lft_pwm = (int) max(min(spd,255),0);
	// analogWrite(M1PWM, lft_pwm);
}

void rgt_frw(float spd)
{
	// digitalWrite(M2AEN, HIGH);
	// digitalWrite(M2AIN, HIGH);
	// digitalWrite(M2BIN, LOW);
	//
	// rgt_pwm = (int) max(min(spd,255),0);
	// analogWrite(M2PWM, rgt_pwm);
}

void process_IR_cmd(uint8_t cmd)
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

	old_z = GY85.gyro_z(GY85.readGyro());
	for (int i=0; i < 10; i++)
	{
		delay(300);
		new_z = GY85.gyro_z(GY85.readGyro());
		delta += new_z - old_z;
	}
	delta /= 10;

	pinMode(M1AIN, OUTPUT);
	pinMode(M1BIN, OUTPUT);
	pinMode(M1PWM, OUTPUT);
	pinMode(M1AEN, OUTPUT);
	pinMode(M1BEN, OUTPUT);

	pinMode(M2AIN, OUTPUT);
	pinMode(M2BIN, OUTPUT);
	pinMode(M2PWM, OUTPUT);
	pinMode(M2AEN, OUTPUT);
	pinMode(M2BEN, OUTPUT);

	pinMode(30, INPUT);
	pinMode(31, INPUT);
	pinMode(32, INPUT);
}

uint8_t readIRC()
{
	uint8_t res;
	if(digitalRead(SYNC))
	{
		res = B10000111 & PORTC;
		digitalWrite(READY, 0);
	}
	else
	{
		digitalWrite(READY, 1);
		res = NOCMD;
	}
	return res;
}


void loop()
{
	pure_gz = GY85.gyro_z(GY85.readGyro());
	gz += (pure_gz - delta) * dt;

	cmd = readIRC();
	process_IR_cmd(cmd);

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
