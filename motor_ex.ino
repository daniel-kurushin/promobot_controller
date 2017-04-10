#include <Wire.h>

#include <GY_85.h>

#define M1AIN 7
#define M1BIN 6
#define M1PWM 5
#define M1AEN 4
#define M1BEN 3
#define M2AIN 8
#define M2BIN 9
#define M2PWM 10
#define M2AEN 11
#define M2BEN 12

GY_85 GY85;
float gz = 0;
float pure_gz = 0;
float dt = 0.01;
float delta = 0.0;
float old_z = 0;
float new_z = 0;

void setup() {
  Wire.begin();
  delay(10);
  Serial.begin(9600);
  delay(10);
  GY85.init();
  delay(10);
  old_z = GY85.gyro_z( GY85.readGyro() );
//  Serial.println(old_z);
//  Serial.println("###");
  for (int i=0; i < 10; i++) {
    delay(300);
    new_z = GY85.gyro_z( GY85.readGyro() );
    
//    Serial.println(new_z);
    
    delta += new_z - old_z;
  }
//  Serial.println("###");
  delta /= 10; 
//  Serial.println(delta);
    
  pinMode(M1AIN,OUTPUT);
  pinMode(M1BIN,OUTPUT);
  pinMode(M1PWM,OUTPUT);
  pinMode(M1AEN, OUTPUT);
  pinMode(M1BEN, OUTPUT);

  pinMode(M2AIN,OUTPUT);
  pinMode(M2BIN,OUTPUT);
  pinMode(M2PWM,OUTPUT);
  pinMode(M2AEN, OUTPUT);
  pinMode(M2BEN, OUTPUT);
}

int lVal = 0;
//float lValPure = 0.0;
int rVal = 0;
//float rValPure = 0.0;
void loop() {
  pure_gz=GY85.gyro_z( GY85.readGyro() ) * dt;
  gz += pure_gz - delta*dt*0.6;
  
//  lValPure = 0;
//  for (int i = 0; i < 8; i++) {
//    lValPure += analogRead(A0); 
//  }
//  lVal = (int)(lValPure / 8) - 512;
//  Serial.println(lVal);
  lVal = 256;
  if (lVal > 0) {
    digitalWrite(M1AEN, HIGH);
    digitalWrite(M1AIN, HIGH);
    digitalWrite(M1BIN, LOW);
  } else if (lVal < 0) {
    digitalWrite(M1BEN, HIGH);
    digitalWrite(M1AIN, LOW);
    digitalWrite(M1BIN, HIGH); 
  } else if (lVal == 0) {
    digitalWrite(M1AEN, LOW);
    digitalWrite(M1BEN, LOW);
  }
  analogWrite(M1PWM, (int) abs(lVal/2));
  
//  rValPure = 0;
//  for (int i = 0; i < 8; i++) {
//    rValPure += analogRead(A1); 
//  }
//  rVal = (int)(rValPure / 8) - 512;
//  Serial.println(rVal);
  rVal = 256;
  if (rVal > 0) {
    digitalWrite(M2AEN, HIGH);
    digitalWrite(M2AIN, HIGH);
    digitalWrite(M2BIN, LOW);
  } else if (rVal < 0) {
    digitalWrite(M2BEN, HIGH);
    digitalWrite(M2AIN, LOW);
    digitalWrite(M2BIN, HIGH); 
  } else if (rVal == 0) {
    digitalWrite(M2AEN, LOW);
    digitalWrite(M2BEN, LOW);
  }
  analogWrite(M2PWM, (int) (abs(rVal/2) + gz));
  Serial.println(abs(lVal/2));
  Serial.println((int) (abs(rVal/2) + gz));
  Serial.println(gz);
  Serial.println(delta*dt);
  
  delay(dt * 1000);
}
