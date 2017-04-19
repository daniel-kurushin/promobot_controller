#include <IRremote.h>

typedef enum direction {
	LEFT=16716015,
	RIGHT=16734885,
	FWD=16718055,
	BACK=16730805,
	STOP=16726215
} Direction;

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;
uint32_t dir_recieved = 0;
// Direction direction;

void setup()
{
	Serial.begin(115200);
	irrecv.enableIRIn(); // Start the receiver
}

void loop() {
	if (irrecv.decode(&results)) {
		// Serial.println(results.value);
		dir_recieved = results.value;

		switch (dir_recieved){

			case LEFT:
				Serial.println("LEFT");
				//
				break;
			case RIGHT:
				Serial.println("RIGHT");
				// add 10 to right wheel pwm value, substract 10 from left value
				break;
			case FWD:
				Serial.println("FWD");
				// need to equal pwm s of 2 wheels?
				// add 10 to all wheels pwm values
				break;
			case BACK:
				Serial.println("BACK");
				// NOT IMPLEMENTED
				break;
			case STOP:
				Serial.println("STOP");
				// all pwm and Kp = 0
				break;
		}
		irrecv.resume(); // Receive the next value
	}
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 ~ ^^ (back)
// 16726215 . (stop)