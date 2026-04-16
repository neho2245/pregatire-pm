#define F_CPU 8000000L // Clock Speed
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
#define ANIMATION_TIME 3000

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

// this counts systicks
void timer1_init() {
  // put timer1 on CTC mode
  TCCR1B |= (1 << WGM12);
  
  // set prescaler to 8
  TCCR1B |= (1 << CS11);

  // set OCR1A to 1000 (this counts 1ms)
  OCR1A = 1000;

  // !!! enable interrupt
  TIMSK1 |= (1 << OCIE1A);

  // enable interrupts on timer B as well
  TIMSK1 |= (1 << OCIE1B);

  // it works like this:
  // the counter starts counting, it gets to OCR1B and triggers and interrupt
  // then, it goes to OCR1A and trigger another interrupt
  // this way, the LED is off betweeen OCR1B and OCR1A (on -> OCR1B -> off -> OCR1A -> on ...)
  OCR1B = 500;
}

volatile int systicks = 0;
volatile int last_ping = 0;
volatile int print_time = 0;
ISR(TIMER1_COMPA_vect) {
  systicks++;
  if (systicks - last_ping > 999) {
    last_ping = systicks;
    print_time = 1;
  }

  // also activate the blue LED on CTC
  PORTD |= (1 << PD5);
}

ISR(TIMER1_COMPB_vect) {
  PORTD &= ~(1 << PD5);
}

// fast PWM mode
void timer0_init() {
  // set fast PWM with reset toggle on OCRA but reset on 0xFF
  TCCR0A |= (1 << WGM01);
  TCCR0A |= (1 << WGM00);

  // set prescaler to 64
  TCCR0B |= (1 << CS01);
  TCCR0B |= (1 << CS00);

  // set NON inverting mode
  // OC0A is the pin (set on CTC and clear at bottom)
  // TCCR0A |= (1 << COM0A0);
  TCCR0A |= (1 << COM0A1);

  // set OCR0A to 0 initially
  OCR0A = 0;
}

void led_init() {
  // activate red LED
  DDRB |= (1 << PB3);

  // activate blue LED
  DDRD |= (1 << PD5);
}

void button_init() {
  // activate pull-up resistance
  PORTB |= (1 << PB2);

  // activate interrupts (set it to trigger on falling edge for INT2)
  EICRA |= (1 << ISC21);

  // activate INT2
  EIMSK |= (1 << INT2);
}

volatile int button_pressed = 0;
volatile int last_button_press = 0;
ISR(INT2_vect) {
  if (systicks - last_button_press > 49) {
    button_pressed = 1;
  }
}


int main() {
  USART_Init(MYUBRR);

  USART_Send_String("hello!");

  // init timer 1
  timer1_init();

  // init timer 0
  timer0_init();

  // init led
  led_init();

  // init button
  button_init();

  // set OCR0A value range
  int button_presses = 0;
  unsigned char led_values[5] = {0, 64, 128, 192, 255};

  // enable interrupts
  sei();

  while (1) {
    if (print_time) {
      char msg[50];
      sprintf(msg, "Time is %d\n", OCR0A);
      USART_Send_String(msg);
      print_time = 0;
    }

    if (button_pressed) {
      USART_Send_String("Button pressed!");
      button_pressed = 0;
      button_presses++;
      OCR0A = led_values[button_presses % 5];
    }
  }

  return 0;
}