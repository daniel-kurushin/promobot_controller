#include <Wire.h>
#include <Multiservo.h>

#define LH_NOT_LIMIT (PINB & _BV(LFTSTSWPIN)) == 1
#define LH_LIMIT !LH_NOT_LIMIT
#define RH_NOT_LIMIT (PINB & _BV(RGTSTSWPIN)) == 1
#define RH_LIMIT !RH_NOT_LIMIT

#define LH_STOP PORTG &= ~_BV(LEFTHAND)
#define RH_STOP PORTG &= ~_BV(RIGHTHAND)

#define RIGHTHAND PG1 // 40
#define LEFTHAND PG0 // 41
#define RHUP PL4 // 45
#define LHUP PL5 // 44
#define RHDOWN PL6 // 43
#define LHDOWN PL7 // 42

int cmd = 0;

void hands_up() {
	int cnt = 0;
	while (LH_NOT_LIMIT | RH_NOT_LIMIT) {
		PORTL |= _BV(LHUP); PORTL &= ~_BV(LHDOWN); PORTG |= _BV(LEFTHAND);
		PORTL |= _BV(RHUP); PORTL &= ~_BV(RHDOWN); PORTG |= _BV(RIGHTHAND);
		if (cnt++ > 10000) break;
	}
}

void hands_down() {
	int cnt = 0;
	while (LH_NOT_LIMIT | RH_NOT_LIMIT) {
		PORTL |= _BV(LHDOWN); PORTL &= ~_BV(LHUP); PORTG |= _BV(LEFTHAND);
		PORTL |= _BV(RHDOWN); PORTL &= ~_BV(RHUP); PORTG |= _BV(RIGHTHAND);
		if (cnt++ > 10000) break;
	}
}

void fingers_push() {

}

void fingers_release() {

}

void setup() {
	Serial.begin(9600);
	DDRB |= _BV(LFTSTSWPIN) | _BV(RGTSTSWPIN);
	DDRL |= _BV(RHUP) | _BV(LHUP) | _BV(RHDOWN) | _BV(LHDOWN);
	DDRG |= _BV(RIGHTHAND) | _BV(LEFTHAND);
}

void loop() {
	if (Serial.available()) {
		cmd = Serial.parseInt();
		switch(cmd) {
			case 1:
				hands_up();
				break;
			case 2:
				hands_down();
				break;
			case 3:
				fingers_push();
				break;
			case 4:
				fingers_release();
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			default:
				break;
		}
	}
}