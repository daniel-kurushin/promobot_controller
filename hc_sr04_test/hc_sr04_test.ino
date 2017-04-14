int echoPinA = 11;
int trigPinA = 12;
int echoPinB = 11;
int trigPinB = 12;
//            0      1     2     3      4      5
typedef enum {
  BEGIN, TRIG, WAIT, ECHOR, ECHOL, ERROR1, ERROR2} 
tstt;

volatile uint8_t  err = 0;
volatile tstt     stt = BEGIN;
volatile uint16_t tme = 0;
volatile uint16_t dur = 0;


void setup()
{
  pinMode(8, OUTPUT);

  Serial.begin (115200);
  pinMode(trigPinA, OUTPUT);
  pinMode(echoPinA, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

  // attachInterrupt(0, right_t, RISING);
  // attachInterrupt(1, left_t, RISING);

  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
  TCNT0 = 0;
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
      if(tme++ > 1)
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
        digitalWrite(8,1);
      }
      if(tme++ > 6000)
      {
        tme = 0;
        stt = ERROR1;
      }
      break;
    }
  case ECHOL:
    {
      if(digitalRead(echoPinA) == 0)
      {
        stt = BEGIN;
        dur = tme;
        tme = 0;
        digitalWrite(8,0);
      }
      if(tme++ > 60000)
      {
        tme = 0;
        stt = ERROR2;
      }
      break;
    }
  case ERROR1:
    {
      dur = tme + 1;
      tme = 0;
      stt = BEGIN;
      err++;
      break;
    }
  case ERROR2:
    {
      dur = tme + 2;
      tme = 0;
      stt = BEGIN;
      err++;
      break;
    }
  }
}

int duration, cm;

void loop()
{
  uint16_t i = 0;
  while(i++ < 60000);
  Serial.print(" dur = ");
  Serial.println(dur);
}

