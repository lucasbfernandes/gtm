#define BAUD 2400
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define SYNC 0XAA// synchro signal
#define SYNCE 0xFF //end sync
#define RADDR 0x44

#define DATA 'A'

int ledPin = 13;

void initUSART(void) {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif

  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void transmitByte(uint8_t data) {
  while ((UCSR0A & (1 << UDRE0)) == 0);
  UDR0 = data;
}

void Send_Packet(uint8_t addr, uint8_t data, uint8_t sync)
{
  transmitByte(sync); //send SYNC signal
  transmitByte(addr); //send addr Byte
  transmitByte(data); //send Data (what is actually being transmitted)
  transmitByte(data);
  transmitByte((addr + data)); //sending checksum
  _delay_ms(2);
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

uint16_t* readADC (void)
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
	uint16_t adcValues[6];
	uint16_t *p;
	uint8_t d;
	
	//Variable to go through send array
	uint8_t sendData;
	
	//Set analog input pins to input
	//DDRC = 0b00000000;
	
	initUSART();
	initADC();
	
    while(1)
    {
        p = readADC();
		
		
		//sprintf(); //TODO Convert our output   --- B####E ---
		
		
		//transmitByte('S');
		Send_Packet(RADDR, 'S', SYNC);

		for (sendData = 0; sendData < 3; sendData++) {
			
			temp = *(p + sendData);
			
			
			if(sendData == 1) {
				if((temp >= 0b0000000000000000) && (temp <= 0b0000000001000000)) { d = 48;   } //Between zero and 128
				else if ((temp > 0b0000000001000000) && (temp <= 0b0000000010111000)) { d = 49; }
				else { d = 50; } //Greater and 128
			}
			else if(sendData == 2) {
				if((temp >= 0b0000000000000000) && (temp <= 0b0000000001000000)) { d = 65;   } //Between zero and 128
				else if ((temp > 0b0000000001000000) && (temp <= 0b0000000010111000)) { d = 66; }
				else { d = 67; } //Greater and 128
			}
			else {
				if((temp >= 0b0000000000000000) && (temp <= 0b0000000001000000)) { d = 88;   } //Between zero and 128
				else if ((temp > 0b0000000001000000) && (temp <= 0b0000000010111000)) { d = 89; }
				else { d = 90; } //Greater and 128
			}
				
			//transmitByte(d);
                        Send_Packet(RADDR, d, SYNC);
			
			_delay_ms(10);
		}
		
		Send_Packet(RADDR, 'E', SYNC);
                //Send_Packet(RADDR, '\n', SYNC);
		
		
		_delay_ms(50);
    }
}

