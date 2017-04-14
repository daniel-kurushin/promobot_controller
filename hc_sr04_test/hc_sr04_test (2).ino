int echoPinA = 11;
int trigPinA = 12;
int echoPinB = 11;
int trigPinB = 12;
//            0      1     2     3      4      5
typedef enum {BEGIN, TRIG, WAIT, ECHOR, ECHOL, ERROR} tstt;

volatile tstt     stt = BEGIN;
volatile uint16_t tme = 0;
volatile uint16_t dur = 0;


void setup()
{
  Serial.begin (115200);
  pinMode(trigPinA, OUTPUT);
  pinMode(echoPinA, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

	// attachInterrupt(0, right_t, RISING);
  // attachInterrupt(1, left_t, RISING);

	OCR0A = 0x02;
	TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
	Serial.print(" ");
	Serial.print(stt);
	switch (stt)
	{
		case BEGIN:
		{
			digitalWrite(trigPinA, 1);
			stt = TRIG;
			break;
		}
		case TRIG:
		{
			digitalWrite(trigPinA, 0);
			stt = WAIT;
			tme = 0;
			break;
		}
		case WAIT:
		{
			if(tme++ > 2)
			{
				tme = 0;
				stt = ECHOR;
			}
			break;
		}
		case ECHOR:
		{
			if(digitalRead(echoPinA))
			{
				tme = 0;
				stt = ECHOL;
			}
			if(tme++ > 60)
			{
				tme = 0;
				stt = ERROR;
			}
			break;
		}
		case ECHOL:
		{
			if(!digitalRead(echoPinA))
			{
				stt = BEGIN;
				dur = tme;
				tme = 0;
			}
			if(tme++ > 60000)
			{
				tme = 0;
				stt = ERROR;
			}
			break;
		}
	}
}

int duration, cm;

void loop()
{
	Serial.println();
	Serial.print("dur = ");
	Serial.println(dur);
	delay(1000);
}
