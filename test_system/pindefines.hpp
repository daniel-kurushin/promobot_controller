#include <stdint.h>
#define COMP_PWR_REL_DDR DDRB
#define COMP_PWR_BTN_DDR DDRB
#define COMP_PWR_REL_PIN PB7
#define COMP_PWR_BTN_PIN PB6
#define COMP_PWR_REL_PORT PORTB
#define COMP_PWR_BTN_PORT PORTB

#define LAMP_RED_PORT PORTH
#define LAMP_RED_DDR DDRH
#define LAMP_RED_PIN PH6

#define LAMP_WHT_PORT PORTH
#define LAMP_WHT_DDR DDRH
#define LAMP_WHT_PIN PH6

#define M1AIN PA0 // 22 -> 1 (IN A on scheme)
#define M1BIN PA1 // 23 -> 6
#define M1PWM PA2 // 24 -> 5

#define M2AIN PC4 // 33 -> 1
#define M2BIN PC3 // 34 -> 6
#define M2PWM PC2 // 35 -> 5

#define LFTSTSWPIN PB0
#define RGTSTSWPIN PB1

#define LH_NOT_LIMIT (PINB & _BV(LFTSTSWPIN)) == 1
#define LH_LIMIT !LH_NOT_LIMIT
#define RH_NOT_LIMIT (PINB & _BV(RGTSTSWPIN)) == 1
#define RH_LIMIT !RH_NOT_LIMIT

#define SONAR_DATA_LEN 3
#define SONAR_COUNT 2

#define TRIG_1_PIN PB0 //53
#define ECHO_1_PIN PB1 //52
#define TRIG_2_PIN PB3 //50
#define ECHO_2_PIN PB2 //51