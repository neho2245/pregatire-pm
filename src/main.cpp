#define F_CPU 8000000L // Clock Speed
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>

// !!! ON PROTEUS SIMULATOR, UART WORKS ONLY WITH 1 STOP BIT!

enum {
  on,
  off,
  red,
  blink
};

void USART_Init( unsigned int ubrr)
{
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;


  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  /* Set frame format: 8data, 1stop bit */
  UCSR0C = (3<<UCSZ00);

}

void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
  while ( !( UCSR0A & (1<<UDRE0)) )
  ;
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

unsigned char USART_Receive( void )
{
/* Wait for data to be received */
  while ( !(UCSR0A & (1<<RXC0)) )
  ;
  /* Get and return received data from buffer */
  return UDR0;
}

void USART_Send_String (const char* data) {
  while (*data != '\0') {
    USART_Transmit(*data);
    data++;
  }
}

void LED_Init() {
  DDRD |= (1 << PD7);
  DDRD |= (1 << PD5);
  DDRB |= (1 << PB3); 
}

int main() {

  // initialize usart parameters
  USART_Init(MYUBRR);
  LED_Init();
  
  USART_Send_String("hello!");

  while (1) {
  unsigned char command = USART_Receive();
  // USART_Transmit(command);
  // USART_Transmit(command);

  switch (command - '0') {
    case on:
      USART_Transmit(command);
      PORTD |= (1 << PD7);
      PORTD |= (1 << PD5);
      PORTB |= (1 << PB3);
      break;
    
    case off:
      PORTD &= ~(1 << PD7);
      PORTD &= ~(1 << PD5);
      PORTB &= ~(1 << PB3);    
      break;
    
    case red:
      PORTD &= ~(1 << PD7);
      PORTD &= ~(1 << PD5);
      PORTB |= (1 << PB3);
      break;
    
    case blink:
      char led_status[3];
      led_status[0] = PINB & (1 << PB3); // red
      led_status[1] = PIND & (1 << PD7); // green
      led_status[2] = PIND & (1 << PD5); // blue

      for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
          if (led_status[i]) {
            switch (i) {
              case 0:
                PORTB ^= (1 << PB3);
                break;
              case 1:
                PORTD ^= (1 << PD7);
                break;
              case 2: 
                PORTD ^= (1 << PD5);
                break;
              default:
                break;
            }
          }
        }
        _delay_ms(500);
      }
      break;
    default:
      break;
  }
  }

  return 0;
}