#define F_CPU 8000000L // Clock Speed
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
#define RESOLUTION 1024.0
#define VREF 5.0

#define LED_ANIMATION_TIME 3000
enum LED_ANIMATION {
    GLOW_UP = 0,
    GLOW_DOWN,
};

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

char write_buffer[30];
char read_buffer[30];
uint8_t recived_data = 0;
int animation = 0;

void USART0_init(uint16_t ubrr)
{
    /* baud rate registers */
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    /* enable TX and RX */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* frame format: 8 bits, 2 stop, no parity */
    UCSR0C |= (3 << UCSZ00);
}

/*
 * Receives a byte from USART.
 *
 * @return the data byte received;
 */
char USART0_receive(void)
{
    /* busy wait until reception is complete */
    while (!(UCSR0A & (1 << RXC0)));

    /* the received byte is read from this register */
    return UDR0;
}

/*
 * Transmit a byte through the USART.
 *
 * @param `data`: the character to send
 */
void USART0_transmit(char data)
{
    /* wait until buffer is empty */
    while (!(UCSR0A & (1 << UDRE0)));

    /* by writing to this register, transmission hardware is triggered */
    UDR0 = data;
}

/*
 * Trasmits a null-terminated string through the USART.
 *
 * @param str: the string to send
 */
void USART0_print(const char *str)
{
    while (*str != '\0')
        USART0_transmit(*str++);
}

void Timer1_init_systicks(void)
{
    /* Clear previous settings */
    TCCR1A = 0;
    TCCR1B = 0;

    /* Set CTC mode */
    TCCR1B |= (1 << WGM12);

    /* Set prescaler to 8 */
    TCCR1B |= (1 << CS11);

    /* Activate Compare A interrupt */
    TIMSK1 |= (1 << OCIE1A); 

    /* 8Mhz / 8 => 1500 kHz */
    OCR1A = 1000;

    /* TODO Task 2: don't forget something */

}

volatile int systicks = 0;
ISR(TIMER1_COMPA_vect)
{
  systicks++;
}

volatile uint8_t btn1_flag = 0;
volatile int last_press = 0;
ISR(PCINT1_vect)
{
    if (PINB & (1 << PB2)) {
      if (systicks - last_press > 99) {
          // toggle blue
          btn1_flag = 1;
          last_press = systicks;
      }
    }
}

static void GPIO_init(void)
{
    // BTN1 = PB2 input with pull-up
    DDRB &= ~(1 << PB2);
    PORTB |= (1 << PB2);

    // LED = PB3 output
    DDRB |= (1 << PB3);
    PORTB &= ~(1 << PB3);

    // enable LED PD7
    DDRD |= (1 << PD7);
    PORTD |= (1 << PD7);
}

char USART_exec(char* command){
    /* se verifica daca sirul de caractere este: */
  
    if (strlen(command) <= 3){
      sprintf(write_buffer, "s-a primit comanda \"on\"\r\n"); 
      USART0_print (write_buffer);
      OCR0A = 0.2 * 255;
    } else if (strlen(command) <= 5) {
      sprintf(write_buffer, "s-a primit comanda \"blue\"\r\n"); 
      USART0_print (write_buffer);
      OCR2A = 0.6 * 255;
    } else {
      OCR2A = 0;
      animation = systicks;
    }
}

char USART0_data_available() {
    return 1;
}

void USART0_receive_until_newline(char* received_data)
{
    char received_char;
    int index = 0;
    
    // se verifica daca exista date disponibile in buffer-ul de receptie
    if (USART0_data_available()){ 

    /* se citesc datele caracter cu caracter pana la '\n'. daca nu este intalnit '\n', programul ramane blocat aici! */
    while ((received_char = USART0_receive()) != '$') {
        USART0_transmit(received_char);
        received_data[index++] = received_char;
    }

    received_data[index] = '\n';  // adauga '\n'
    received_data[++index] = '\0'; // adauga terminatorul de string la sfarsit

    // se seteaza un flag care indica faptul ca s-au receptionat date cu scopul de a le prelucra
    recived_data = 1;
    // se afiseaza datele primite:
    sprintf(write_buffer, "Date primite: ");
    USART0_print (write_buffer);
    USART0_print (received_data);
    }
}

void Timer0_init_pwm(void)
{
    /* TODO Task 2: initialize timer 0 (different from timer 2, look in datasheet) */

    /* Clear previous settings */
    TCCR0A = 0;
    TCCR0B = 0;

    /* Set Fast PWM (8 bits) */
    TCCR0A |= (1 << WGM00);
    TCCR0A |= (1 << WGM01);

    /* Set inverting output for OC0A */
    // TCCR0A |= (1 << COM0A0);
    TCCR0A |= (1 << COM0A1);
    

    /* Set prescaler to 64 */
    TCCR0B |= (1 << CS01) | (1 << CS00);

    /* Set duty cycle to 0%; TOP = 255 */
    OCR0A = 0;
}

void Timer2_init_pwm(void)
{
    /* Clear previous settings */
    TCCR2A = 0;
    TCCR2B = 0;

    /* TODO Task 1: set Fast PWM (8 bits) */
    TCCR2A |= (1 << WGM20);
    TCCR2A |= (1 << WGM21);


    /* TODO Task 1: set inverting output for OC2A */
    TCCR2A |= (1 << COM2A1);


    /* TODO Task 1: set prescaler to 64 */
    TCCR2B |= (1 << CS22);


    /* Set duty cycle to 0%; TOP = 255 */
    OCR2A = 0;
}

int main(void)
{
    GPIO_init();
    USART0_init(MYUBRR);
    Timer1_init_systicks();
    Timer0_init_pwm();
    Timer2_init_pwm();

    PCICR  |= (1 << PCIE1);    // enable PCINT for PORTB
    PCMSK1 |= (1 << PCINT10);   // enable PB2
    PCIFR  |= (1 << PCIF1);    // clear pending flag

    sei();

    USART0_print("hello!");
    while(1) {
      USART0_receive_until_newline(read_buffer);
      USART_exec(read_buffer);
      while (systicks - animation < 4999) {
        OCR2A = (systicks % LED_ANIMATION_TIME) * 255 / LED_ANIMATION_TIME;
      }
      animation = 0;
      OCR2A = 0;
    }
}