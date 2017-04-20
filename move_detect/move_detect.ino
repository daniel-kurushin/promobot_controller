
#define DETECT_PIN 8

void setup()
{
	Serial.begin(9600);
	pinMode(DETECT_PIN, INPUT);
}

uint8_t i = 0;
void loop() {
	Serial.print(digitalRead(DETECT_PIN));
	if(i++ > 80)
	{
		i = 0;
		Serial.println();
	}
}


// 16716015 << (left)
// 16734885 >> (right)
// 16718055 ^^ (fwd)
// 16730805 ~ ^^ (back)
