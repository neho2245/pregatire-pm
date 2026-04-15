#define F_CPU 8000000L // Clock Speed
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// !!! ON PROTEUS SIMULATOR, UART WORKS ONLY WITH 1 STOP BIT!

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

void init_timer0() {
  // set it to clear on compare match
  TCCR0A |= (1 << COM0A1);

  // set prescaler
  TCCR0B |= (1 << CS00);
  TCCR0B |= (1 << CS01);

  // enable interrupts on channel A (we can have different values on channel A and B)
  TIMSK0 |= (1 << OCIE0A);

  // set overflow value
  OCR0A = 125;
}

volatile int uptime_ms = 0;
volatile char print_time = 0;
volatile int last_time = 0;

ISR(TIMER0_COMPA_vect) {
  uptime_ms += 1;
  if (uptime_ms - last_time > 249) {
    last_time = uptime_ms;
    print_time = 1;
  }
}

int main() {
  USART_Init(MYUBRR);

  USART_Send_String("hello!");

  // enable interrupts
  sei();

  // enable timer0
  init_timer0();

  while (1) {
    if (print_time) {
      char message[50];
      sprintf(message, "Time is: %d", uptime_ms);
      USART_Send_String(message);
      print_time = 0;
    }
  }

  return 0;
}