#include "legs.hpp"
#include "Arduino.h"

int course = 0;
int distance = 0;
uint8_t last_cmd = 0;
uint8_t state = 0;

void processLegs(char *resp_buf, uint8_t cmd)
{
	switch (cmd)
	{
		case 21:
			course = Serial.parseInt();
			distance = Serial.parseInt();
			last_cmd = cmd;
			// TODO: change this to recieve cmd from computer
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
			break;
		case 20:
			break;
	}
	sprintf(resp_buf, "%d %d %d %d", last_cmd, state, course, distance);
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

void legsLoop()
{
	// TODO: move to processLegs ?
	pure_gz = GY85.gyro_z(GY85.readGyro());
	gz += (pure_gz - delta) * dt;
	cmd = readIRC();
	if (cmd != NOCMD)
	{
		// get command ?
	}
	
	lft_frw(trg_spd + Kp * atan(z - gz));
	rgt_frw(trg_spd - Kp * atan(z - gz));
	if(++tme > 100)
	{
		tme = 0;
	}

	delay(dt * 1000);
}