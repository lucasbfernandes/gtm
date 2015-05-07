/*
 * MuscleSensorSerialCom.c
 *
 *Device = ATMega328p
 *
 * Created: 3/19/2015 7:51:37 AM
 *  Author: Josh G
 */ 

#define F_CPU 16000000
#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void initUSART(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	
	#if USE_2X
	  UCSR0A |= (1 << U2X0);
	#else
	  UCSR0A &= ~(1 << U2X0);
	#endif

	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 Data bits one stop bit
}

void transmitByte(uint8_t data)
{
	while ((UCSR0A & (1<<UDRE0)) == 0); //Waiting for empty transfer buffer
	UDR0 = data;
}

void initADC (void) 
{
	ADMUX |= (1<<REFS0);
	
	ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (ADPS0));
	
	//From example in class this should correlate to pc5/analog in 5 Multiple inputs in 147
	ADMUX |= ((1<<MUX2) | (1<<MUX0));
	
	//Power on ADC
	ADCSRA |= (1<<ADEN);
}

uint16_t readADC (void)
{
	uint8_t channel;
	uint16_t adcValues[6];
	uint16_t *p;
	
	for (channel = 0; channel < 6; channel++) {
		ADMUX = (0xf0 & ADMUX) | channel; // set channel
		
		ADCSRA |= (1 << ADSC); // start next conversion
		
		//Wait until the conversion is done/ADSC == 0
		while (!((ADCSRA & (1<<ADSC)) == 0));
		
		adcValues[channel] = ADC; // store the value in array
	}
	
	p = adcValues;
	
	return(p);
}

int main(void)
{
	//Variable used for transmitting data
	uint8_t data = 0b00000000;
	uint16_t temp = 0b0000000000000000;
	
	//The array and stuff for reading multiple channels
	uint16_t adcValuess[6];
	uint8_t *p;
	uint16_t *d;
	
	//Set analog input pins to input
	//DDRC = 0b00000000;
	
	initUSART();
	initADC();
	
    while(1)
    {
        p	= readADC();
		
		*d = *(p+5);
		
		//Should take the last 8 bits of data from the 16 bit ADC
		//d = p + 5;
		*d = 0b11111111 & temp;
		
		//This should give me the first 8 bits instead
		//unsigned  mask;
		//mask = ((1 << 7) - 1) << 0;
		
		//sprintf(); //TODO Convert our output   --- B####E ---
		
		uint8_t this = 1;
		
		if((temp >= 0b00000000) && (temp <= 0b01000000)) { this = 65;   } //Between zero and 128
		else if ((temp > 0b01000000) && (temp <= 0b0000000010111000)) { this = 50; }
		else { this = 126; } //Greater and 128
		
		//transmitByte(this);
		
		transmitByte(d);
		
		_delay_ms(250);
    }
}