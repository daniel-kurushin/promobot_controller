#include <Wire.h>
#include <GY_85.h>
#include <math.h>
#include <IRremote.h>
#define IR_USE_TIMER2

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
	digitalWrite(M1AEN, HIGH);
	digitalWrite(M1AIN, HIGH);
	digitalWrite(M1BIN, LOW);
	
	lft_pwm = (int) max(min(spd,255),0);
	analogWrite(M1PWM, lft_pwm);
}

void rgt_frw(float spd)
{
	digitalWrite(M2AEN, HIGH);
	digitalWrite(M2AIN, HIGH);
	digitalWrite(M2BIN, LOW);
	
	rgt_pwm = (int) max(min(spd,255),0);
	analogWrite(M2PWM, rgt_pwm);
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
	
  pinMode(13, OUTPUT);

	pinMode(30, INPUT);
	pinMode(31, INPUT);
	pinMode(32, INPUT);
}

// bool readSynchro()
// {
//   bool res = false;
//   if (Serial1.available())
//   {
//     res = Serial1.read() == SYNC_BYTE;
//   }
//   return res;
// }

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
