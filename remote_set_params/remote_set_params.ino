#include <IRremote.h>

#define LEFT	16716015
#define RIGHT	16734885
#define FWD		16718055
#define BACK	16730805
#define STOP	16726215

#define READY	0

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;
uint32_t ircommand = STOP;

void setup()
{
	DDRB |= B00000111;
	PORTB = 0;
	irrecv.enableIRIn(); // Start the receiver
}

void loop() {
	if (irrecv.decode(&results)) {
		if (digitalRead(READY))
		{
			ircommand = results.value;
			switch (ircommand) // 64 is B01000000 -> SYNC bit is 1, others are 0
			{
				case LEFT: // 5
					PORTB = 64 | 5;
					break;
				case RIGHT: // 4
					PORTB = 64 | 4;
					break;
				case FWD: // 3
					PORTB = 64 | 3;
					break;
				case BACK: // 2
					PORTB = 64 | 2;
					break;
				case STOP: // 1
					PORTB = 64 | 1;
					break;
			}
			irrecv.resume();
		}
	}
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 vv (back)
// 16726215 .. (stop)
