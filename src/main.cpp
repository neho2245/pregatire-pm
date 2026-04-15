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
  // set it to one less second, because this will trigger when the difference is 1000
  if (uptime_ms - last_time > 999) {
    last_time = uptime_ms;
    print_time = 1;
  }
}

void init_button() {
  // initialize button
  // activate pull-up resistance
  PORTB |= (1 << PB2);

  // enable PCINT[14:8] (this enable the interrupt on the whole vector)
  PCICR |= (1 << PCIE1);
  
  // enable interrupt on the corresponding pin (and this enables the interrupt to trigger on the specific pin)
  // in this case button 2 (which is connected on PCINT10)
  // we need both PCICR and PCMSK in order for the button to work
  PCMSK1 |= (1 << PCINT10);
}

volatile char button_pressed = 0;
volatile char press_time = 0;
ISR(PCINT1_vect) {

  // the interrupt trigger on the whole PCINT[14:8] so we need to check which pin actually generated the interrupt
  // this check that the pin is 0 (which means the button made the connection to ground)
  // there is a 50ms delay
  if (! (PINB & (1 << PB2))) {
    if (uptime_ms - press_time > 49) {
      button_pressed = 1;
      press_time = uptime_ms;
    }
  }
}

int main() {
  USART_Init(MYUBRR);

  USART_Send_String("hello!");

  // enable interrupts
  sei();

  // enable timer0
  init_timer0();

  // enable button interrupt
  init_button();

  while (1) {
    if (print_time) {
      char message[50];
      sprintf(message, "Time is: %d\n", uptime_ms);
      USART_Send_String(message);
      print_time = 0;
    }

    if (button_pressed) {
      USART_Send_String("Button pressed!\n");
      button_pressed = 0;
    }
  }

  return 0;
}