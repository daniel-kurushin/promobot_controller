#include <Wire.h>
#include <Multiservo.h>
 
Multiservo myservo0; //test
Multiservo myservo1; // left hand
Multiservo myservo2; // head 
Multiservo myservo3; // right hand
Multiservo myservo4; // head up/down

int pos = 0;
int num = 0;
void setup(void) {
  Wire.begin();
  Serial.begin(9600);
  myservo0.attach(0);
  myservo1.attach(16);
  myservo2.attach(15);
  myservo3.attach(14);
  myservo4.attach(13);
}
 
void loop(void) {
  if(Serial.available() > 0)
  {
    num = Serial.parseInt();
    pos = Serial.parseInt();
    Serial.println(num);
    Serial.println(pos);
    switch (num)
    {
      case 0:
        myservo0.write(pos);
        break;
        // other servo motors
      case 4:
        myservo4.write(pos);
        break;
      default:
        Serial.println("[ERROR] No such number");
    }
    delay(1000);
  }
}
