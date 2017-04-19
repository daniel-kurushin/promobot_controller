#include <IRremote.h>

#define LEFT	16716015
#define RIGHT	16734885
#define FWD		16718055
#define BACK	16730805
#define STOP	16726215

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
		// Serial.println(results.value);
		ircommand = results.value;
		switch (ircommand){
			case LEFT: // 4
				PORTB = 4;
				break;
			case RIGHT: // 3
				PORTB = 3;
				break;
			case FWD: // 2
				PORTB = 2;
				break;
			case BACK: // 1
				PORTB = 1;
				break;
			case STOP: // 0
				PORTB = 0;
				break;
		}
		irrecv.resume(); // Receive the next value
	}
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 vv (back)
// 16726215 .. (stop)
