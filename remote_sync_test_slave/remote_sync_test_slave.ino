#define LEFT	16716015
#define RIGHT	16734885
#define FWD		16718055
#define BACK	16730805
#define STOP	16726215

#define READY	8

uint32_t ircommand = STOP;

void setup()
{
	DDRB |= B01111111;
	PORTB = 0;

}

void loop() {
	// if (irrecv.decode(&results)) {
		if (digitalRead(READY))
		{
			ircommand = LEFT;
			switch (ircommand) // 16 is B01000000 -> SYNC bit is 1, others are 0
			{
				case LEFT: // 5
					PORTB = 128 | 5;
					break;
				case RIGHT: // 4
					PORTB = 128 | 4;
					break;
				case FWD: // 3
					PORTB = 128 | 3;
					break;
				case BACK: // 2
					PORTB = 128 | 2;
					break;
				case STOP: // 1
					PORTB = 128 | 1;
					break;
			}
			// irrecv.resume();
		}
	// }
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 vv (back)
// 16726215 .. (stop)
