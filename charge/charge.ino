int curValues = 0;
int step = 1;
void setup() {
	Serial.begin(9600);
}

void loop() {
	curValues += analogRead(A0) - curValues/step;
	step++;
	if (step > 10) {
		step = 1;
	}
	Serial.print();
	Serial.print("\t ");
	Serial.print(analogRead(A1)/4);
	Serial.print("\n ");
	delay(500);
}