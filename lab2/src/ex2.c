
#include "exenable.h"


#if NR_EX == 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "usart.h"
#include "uptime.h"

volatile uint8_t btn1_flag = 0;

ISR(PCINT1_vect)
{
    PORTB ^= (1 << PB3);   // toggle blue
    btn1_flag = 1;
}

static void GPIO_init(void)
{
    // BTN1 = PB2 input with pull-up
    DDRB &= ~(1 << PB2);
    PORTB |= (1 << PB2);

    // LED = PB3 output
    DDRB |= (1 << PB3);
    PORTB &= ~(1 << PB3);
}


int main(void)
{
    GPIO_init();
    USART0_init(9600);


    PCICR  |= (1 << PCIE1);    // enable PCINT for PORTB
    PCMSK1 |= (1 << PCINT10);   // enable PB2
    PCIFR  |= (1 << PCIF1);    // clear pending flag

    sei();

    if (PCICR & (1 << PCIE0)) USART0_print("PCIE0 ok\r\n");
    if (PCMSK1 & (1 << PCINT10)) USART0_print("PCINT2 ok\r\n");

    while (1) {
        if (btn1_flag) {
            btn1_flag = 0;
            USART0_print("PRESS\r\n");
        }
    }
}

#endif