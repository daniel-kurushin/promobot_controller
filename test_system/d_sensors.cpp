#include "d_sensors.hpp"
#include "Arduino.h"
#include "pindefines.hpp"
#include <math.h>

#define IDLE 0
#define TRIG 1
#define ECHO 2
#define SETP 3
#define FRUP 4
#define FRDN 5

// #define R1 digitalWrite(10,1)
// #define G1 digitalWrite(11,1)
// #define B1 digitalWrite(12,1)
// #define R0 digitalWrite(10,0)
// #define G0 digitalWrite(11,0)
// #define B0 digitalWrite(12,0)

#define K 4

volatile uint32_t time = 0;
volatile uint32_t ptme = 0;
volatile uint16_t sonar_distance = 0;

volatile uint8_t sonar_state = IDLE;
volatile uint8_t trig_state = IDLE; 
volatile uint8_t echo_state = IDLE; 

/**/
// uint16_t pos =  600;
// uint16_t MIN =    0; 
// uint16_t MAX =  180; 
// int8_t dp = -1;

// uint32_t pos_ptme = 0;

// uint32_t avg = 0;
// uint16_t cnt = 0;
/**/

char out[100];

uint16_t sonarWork()
{
  switch (sonar_state)
  {
    case IDLE: {
      if (time - ptme > 100000) 
      {
        sonar_state = TRIG;
      }
      break;
    }
    case TRIG: {
      switch (trig_state)
      {
        case IDLE: {
          trig_state = FRUP;
          break;
        }
        case FRUP: {
          PORTB |= _BV(TRIG_PIN);
          ptme = time;
          trig_state = FRDN;
          break;
        }
        case FRDN: {
          if (time - ptme > 0)
          {
            PORTB &= ~_BV(TRIG_PIN);
            trig_state = IDLE;
            sonar_state = ECHO;
            echo_state = IDLE;
          }
          break;
        }
      }
      break;
    }
    case ECHO: {
      switch (echo_state)
      {
        case IDLE: {
          if (PINB & _BV(ECHO_PIN))
          {
            ptme = time;
            echo_state = FRUP;
          }
          break;
        }
        case FRUP: {
          if (! (PINB & _BV(ECHO_PIN)))
          {
            sonar_distance = (time - ptme) / K;
            echo_state = FRDN;
          }
          break;
        }
        case FRDN: {
          echo_state = IDLE;
          sonar_state = IDLE;
          break;
        }
      }
      break;
    }
  }
  return sonar_distance;
}

// void setup() 
// { 
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN,  INPUT);
//   pinMode(SERV_PIN, OUTPUT);
//   pinMode(13, OUTPUT);
//   Serial.begin(115200);
//   a0.attach(SERV_PIN);
// } 
// time = micros();
//   if (time < ptme) ptme = time;
//   avg += sonarWork() * 100; cnt++;
// //  servoWork(pos);

//   if (time - pos_ptme > 10000)
//   {
//     pos = map(avg / cnt, 0, 300, MIN, MAX);
//     if (pos < MIN) pos = MIN;
//     if (pos > MAX) pos = MAX;
//     pos_ptme = time;
//     cnt = 0;
//     avg = 0;
//     a0.write(pos);
//   }
//   R0; G0; B0;


