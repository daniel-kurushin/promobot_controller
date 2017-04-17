int echoPinA = 2;
int trigPinA = 12;
int echoPinB = 11;
int trigPinB = 12;

volatile uint16_t tme = 0;
volatile float dur = 0;

void setup()
{
	pinMode(8, OUTPUT);
	Serial.begin (115200);
	pinMode(trigPinA, OUTPUT);
	pinMode(echoPinA, INPUT);
	pinMode(trigPinB, OUTPUT);
	pinMode(echoPinB, INPUT);

	attachInterrupt(0, echo, RISING);
//   // attachInterrupt(1, left_t, RISING);
	OCR2A = 0x0F;
	TIMSK2 |= _BV(OCIE2A);
}

SIGNAL(TIMER2_COMPA_vect)
{
	TCNT2 = 0;
	tme++;
}

void trig()
{
	digitalWrite(trigPinA, 0);
	delayMicroseconds(5);
	digitalWrite(trigPinA, 1);
	delayMicroseconds(10);
	digitalWrite(trigPinA, 0);
}

void echo()
{
	if (digitalRead(echoPinA))
	{
		tme = 0;
		attachInterrupt(0, echo, FALLING);
	}
	else
	{
		dur = tme / 58.0;
		attachInterrupt(0, echo, RISING);
	}
}

void loop()
{
	delay(500);
	trig();
	Serial.print(" dur = ");
	Serial.print(dur);
	Serial.println();
}
