#define BAUD 2400
//Some code courtesy of hekilledmywire.com
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define SYNC 0XAA// synchro signal
#define RADDR 0x44
#define DATA 'A'

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

uint8_t receiveByte(void) {
  while ((UCSR0A & (1 << RXC0)) == 0);
  return UDR0;
}

int main() {
  UCSR0B |= (1 << RXCIE0);
  initUSART();
  //sei();
  while (1)
  {

    if (SYNC == receiveByte()) {
      uint8_t raddress, data,data2, chk;//transmitter address
      //receive destination address
      raddress = receiveByte();
      //receive data
      data = receiveByte();
      data2 = receiveByte();
      //receive checksum
      chk = receiveByte();
      //compare received checksum with calculated
      if (chk == (raddress + data)) //if match perform operations
      {
        //if transmitter address match
        if (raddress == RADDR)
        {
          transmitByte(data);
          //if (data == 'E') transmitByte('\n');
        }
      }
      else
      {
        transmitByte('?');
      }

    }
  }
  return 0;
}

