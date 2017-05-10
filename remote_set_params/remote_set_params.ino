#include <IRremote.h>

#define IR_USE_TIMER2

#define LEFT	16716015
#define RIGHT	16734885
#define FWD		16718055
#define BACK	16730805
#define STOP	16726215

#define LEFT_CMD_REMOTE  0x05
#define RIGHT_CMD_REMOTE 0x04
#define FWD_CMD_REMOTE   0x03
#define BACK_CMD_REMOTE  0x02
#define STOP_CMD_REMOTE  0x01
#define NOCMD_CMD_REMOTE 0x00

#define SYNC_BYTE 0x9e
#define READY	0

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;
uint32_t ircommand = STOP;

void sendCmd(uint8_t cmd) {
	Serial.write(SYNC_BYTE);
	Serial.write(SYNC_BYTE);
	Serial.write(cmd);
}

void setup()
{
	Serial.begin(9600);
	irrecv.enableIRIn(); // Start the receiver
}

void loop() {
	if (irrecv.decode(&results)) {
		ircommand = results.value;
		switch (ircommand)
		{
			case LEFT:
				sendCmd(LEFT_CMD_REMOTE);
				break;
			case RIGHT:
				sendCmd(RIGHT_CMD_REMOTE);
				break;
			case FWD:
				sendCmd(FWD_CMD_REMOTE);
				break;
			case BACK:
				sendCmd(BACK_CMD_REMOTE);
				break;
			case STOP:
				sendCmd(STOP_CMD_REMOTE);
				break;
		}
		irrecv.resume();
	}
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 vv (back)
// 16726215 .. (stop)
