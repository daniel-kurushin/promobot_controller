
#define READY 8
#define SYNC  9

#define LEFT  5
#define RIGHT 4
#define FWD   3
#define BACK  2
#define STOP  1
#define NOCMD 0

uint8_t cmd = 0;

void setup()
{
	Serial.begin(9600);

	DDRB |= B01111111;
}

uint8_t readIRC()
{
    uint8_t res;
    if(digitalRead(SYNC))
    {
            res = B10001111 & PORTB;
            Serial.print(res);
            digitalWrite(READY, 0);
    }
    else
    {
            digitalWrite(READY, 1);
            res = NOCMD;
    }
    return res;
}

void loop()
{
	cmd = readIRC();
	Serial.print("CMD: "); Serial.print(cmd);
    Serial.println();

  	// process_IR_cmd(cmd);
	
	delay(2000);
}