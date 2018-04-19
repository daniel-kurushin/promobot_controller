
#define SSerialTxControl 3   //RS485 Direction control
#define RS485_RECV PF2
#define RS485_TRSM PF3

#define Pin13LED         13

/*-----( Declare objects )-----*/
// SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;

void rs485_trsm_en()
{
  PORTF |= _BV(RS485_TRSM);
  PORTF &= ~_BV(RS485_RECV);
}

void rs485_recv_en()
{
  PORTF &= ~_BV(RS485_TRSM);
  PORTF |= _BV(RS485_RECV);
}

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial1.begin(9600);
  DDRF |= _BV(RS485_TRSM);
  DDRF |= _BV(RS485_RECV);
  
  pinMode(Pin13LED, OUTPUT);
}


void loop()
{
    digitalWrite(Pin13LED, HIGH);  // Show activity
    rs485_recv_en();
    if (Serial1.available())
    {
        byteReceived = Serial1.parseInt();
        
        rs485_trsm_en();
        Serial.println(byteReceived);          // Send byte to Remote Arduino
        digitalWrite(Pin13LED, LOW);  // Show activity
        delay(50);
    }
}
