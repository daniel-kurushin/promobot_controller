volatile uint16_t adc_val = 0;
volatile uint16_t adc_val1 = 0;

void setup() {	
	noInterrupts();
	Serial.begin(9600);
	//ADC setup
	ADCSRA |= (1 << ADPS2)
         | (1 << ADPS1)
         | (1 << ADPS0); // Set ADC prescaler to 128 - 125KHz sample rate @ 16MHz
  	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC

    ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	ADCSRA |= (1 << ADATE);
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADIE);  // Enable ADC Interrupt
	ADCSRB |= (1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0);

	// ADMUX &= ~(1 << MUX0)
	//     &  ~(1 << MUX1)
	//     &  ~(1 << MUX2)
	//     &  ~(1 << MUX3);
	ADMUX = 0x00;
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	interrupts();
}

void loop() {
	Serial.println(adc_val);
	Serial.println(adc_val1);
	delay(500);
}

ISR(ADC_vect)
{
	digitalWrite(13,HIGH);
	ADCSRA |= (1 << ADSC);
	// while(!(ADCSRA & (1<<ADIF)));
	// ADCSRA|=(1<<ADIF);
	adc_val = ADCH;
	adc_val1 = ADCL;
	digitalWrite(13,LOW);
}