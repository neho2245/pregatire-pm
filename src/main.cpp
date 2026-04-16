#define F_CPU 8000000L // Clock Speed
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
#define RESOLUTION 1024.0
#define VREF 5.0



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

void adc_init() {
  // ADMUX IS 0, so it means channel 0 is selected as default
  ADMUX |= (1 << REFS0);

  // set prescaler to 128
  ADCSRA |= (1 << ADPS0) | (1  << ADPS1) | (1 << ADPS2);

  // enable ADC
  ADCSRA |= (1 << ADEN);
}

uint16_t analog_read() {
  // set ADSC to 1 to manually start the conversion
  // after it's done converting, it automatically goes to zero

  ADCSRA |= (1 << ADSC);

  // wait for the previous conversion to end
  while ((ADCSRA & (1 << ADSC)));

  return ADC;
}

int main() {

  //usart init and test
  USART_Init(MYUBRR);
  USART_Send_String("hello!");

  // set PA0 to input
  DDRA &= ~(1 << PA0);

  // enable adc
  adc_init();


  // enable interrupts
  sei();

  uint16_t adc_value = analog_read();
  // we multiply by 100, because each 10mV is 1C on the LM35
  float temp_value = ((adc_value * VREF) / RESOLUTION) * 100; 
  char message[50];
  sprintf(message, "Temp is: %f", temp_value);
  USART_Send_String(message);

  return 0;
}