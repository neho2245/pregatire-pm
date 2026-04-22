#include "exenable.h"

#if NR_EX == 3

#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "usart.h"
#include "uptime.h"


#define BTN1_PIN    PB2
#define BTN2_PIN    PD6
#define BUZZ_PIN    PD4


static void GPIO_init(void)
{
    /* BTN1 = PB2 input + pull-up */
    DDRB &= ~(1 << BTN1_PIN);
    PORTB |= (1 << BTN1_PIN);

    /* BTN2 = PD6 input + pull-up */
    DDRD &= ~(1 << BTN2_PIN);
    PORTD |= (1 << BTN2_PIN);

    /* BUZZER = PD4 output */
    DDRD |= (1 << BUZZ_PIN);
    PORTD &= ~(1 << BUZZ_PIN);
}




/* Buzzer Timer 1*/

volatile uint8_t freq_idx = 0;
volatile uint8_t buzzer_enabled = 0;

const uint16_t freq_table[3] = {200, 400, 800};

ISR(TIMER1_COMPA_vect)
{
    /* Toggle PD4 */
    PIND = (1 << BUZZ_PIN);
}

static void Timer1_set_buzzer_freq(uint16_t freq_hz)
{
    /* Toggle la fiecare compare match:
       OCR1A = F_CPU / (2 * prescaler * freq) - 1
       prescaler = 64
    */
    uint32_t ocr = (F_CPU / (2UL * 64UL * (uint32_t)freq_hz)) - 1UL;
    OCR1A = (uint16_t)ocr;
}

static void Timer1_buzzer_init(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    Timer1_set_buzzer_freq(freq_table[freq_idx]);

    /* CTC, prescaler 64 */
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
}

static void buzzer_start(void)
{
    TIMSK1 |= (1 << OCIE1A);
}

static void buzzer_stop(void)
{
    TIMSK1 &= ~(1 << OCIE1A);
    PORTD &= ~(1 << BUZZ_PIN);
}

/* ISR */

/* BTN1 -> PB2 -> INT2
   Schimbă frecvența */
volatile uint8_t btn1_event = 0;

ISR(INT2_vect)
{
    btn1_event = 1;
}

/* BTN2 -> PD6 -> PCINT30
   Actualizează gate-ul */
volatile uint8_t btn2_changed = 0;

ISR(PCINT3_vect)
{
    btn2_changed = 1;
}

static void INT2_BTN1_init(void)
{
    /* INT2 pe front descendent */
    EICRA &= ~((1 << ISC21) | (1 << ISC20));
    EICRA |=  (1 << ISC21);   // falling edge

    EIFR  |= (1 << INTF2);    // clear flag
    EIMSK |= (1 << INT2);     // enable INT2
}

static void PCINT_BTN2_init(void)
{
    /* PD6 = PCINT30, grupul PORTD => PCIE3 / PCMSK3 / PCINT3_vect */
    PCICR  |= (1 << PCIE3);
    PCMSK3 |= (1 << PCINT30);
    PCIFR  |= (1 << PCIF3);   // clear flag
}



int main(void)
{
    GPIO_init();
    USART0_init(9600);

    uptime_init();
    Timer1_buzzer_init();

    INT2_BTN1_init();
    PCINT_BTN2_init();

    sei();

    uint32_t last_btn1_time = 0;
    bool gate_state = false;

    while (1) {
        uint32_t now = uptime_ms();

        /* BTN1 -> schimbă frecvența, cu debounce */
        if (btn1_event) {
            btn1_event = 0;

            if ((uint32_t)(now - last_btn1_time) >= 30) {
                last_btn1_time = now;

                if (!(PINB & (1 << BTN1_PIN))) {
                    freq_idx = (freq_idx + 1) % 3;
                    Timer1_set_buzzer_freq(freq_table[freq_idx]);

                    USART0_print("Freq changed\r\n");
                }
            }
        }

        /* BTN2 -> gate pentru buzzer */
        if (btn2_changed) {
            btn2_changed = 0;
            gate_state = !(PIND & (1 << BTN2_PIN));
        }

        if (gate_state && !buzzer_enabled) {
            buzzer_enabled = 1;
            buzzer_start();
        }
        else if (!gate_state && buzzer_enabled) {
            buzzer_enabled = 0;
            buzzer_stop();
        }
    }
}

#endif