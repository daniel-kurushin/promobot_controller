#include "legs.hpp"
#include "Arduino.h"
#include "pindefines.hpp"
#include <Wire.h>
#include <GY_85.h>
#include <math.h>

/* legs */
float trg_spd = 0;
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

uint8_t legs_last_cmd = 0;
int trg_course = 0;
int trg_distance = 0;
int course = 0;
int distance = 0;
int direction = 0; // 0 (forward) or 1 (back) TODO: make constants

uint8_t state = 0; // 1 (err) or 0 (ok) TODO: make constants
int avg_pwm = 2;

volatile uint8_t legs_cnt = 0;
volatile uint8_t m1cnt = 0;
volatile uint8_t m2cnt = 0;

void processLegs(char *resp_buf, uint8_t cmd)
{
	switch (cmd)
	{
		case 21: // set course and distance
			trg_course = Serial.parseInt();
			trg_distance = Serial.parseInt();
			direction = 0;
			legs_last_cmd = cmd;
			if (0) // error state
			{
				state = 1;
			}
			sprintf(resp_buf, "legs_last_cmd: %d, state: %d", legs_last_cmd, state);
			break;
		case 20: // get state
			// TODO: compute distance, course 
			course = 0;
			distance = 0;
			legs_last_cmd = cmd;
			sprintf(resp_buf, "legs_last_cmd: %d, avg_pwm: %d, state: %d, direction: %d, trg_course: %d, course: %d, tgr_distance: %d, distance: %d, lft_pwm: %d, rgt_pwm: %d",
			                   legs_last_cmd,     avg_pwm,     state,     direction,     trg_course,     course,     trg_distance,     distance,     lft_pwm,     rgt_pwm);
			break;
		case 22:
			trg_course = Serial.parseInt();
			trg_distance = Serial.parseInt();
			direction = 1;
			legs_last_cmd = cmd;
			if (0) // error state
			{
				state = 1;
			}
			sprintf(resp_buf, "legs_last_cmd: %d, state: %d", legs_last_cmd, state);
			break;
		case 23:
			avg_pwm = 0;
			if (0) // error state
			{
				state = 1;
			}
			legs_last_cmd = cmd;
			sprintf(resp_buf, "legs_last_cmd: %d, state: %d, avg_pwm: %d", legs_last_cmd, state, avg_pwm);
			break;
	}
}

void legsWork()
{
	float spd = avg_pwm * 1.3; // 1.3 depends on wheels radius
	int dt = 1;
	trg_distance -= spd * dt;
	if (trg_distance > 10 && (legs_last_cmd == 21 | legs_last_cmd == 22)) // 10 should be constant
	{
		avg_pwm = 2;
	} else {
		avg_pwm = 0;
	}
}



