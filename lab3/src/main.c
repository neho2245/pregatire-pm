#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "timers.h"
#include "sound.h"
#include "usart.h"

#include <util/delay.h>

/* Constants */
#define PM_BAUD 28800

#define BUZZ_PIN    PD4
#define RED_LED     PD5
#define GREEN_LED   PD7
#define BLUE_LED    PB3
#define BTN1_PIN    PB2
#define BTN2_PIN    PD6

#define LED_ANIMATION_TIME 3000
enum LED_ANIMATION {
    GLOW_UP = 0,
    GLOW_DOWN,
};

#define LED_LEVELS 5

static void GPIO_init(void)
{
    /* TODO Task 1 & 2: initialize LEDs */
    DDRD |= (1 << RED_LED); // red
    PORTD |= (1 << RED_LED); // red OFF
    
    DDRD |= (1 << GREEN_LED); // green
    PORTD |= (1 << GREEN_LED); // green OFF 

    DDRB |= (1 << BLUE_LED); // blue
    PORTB |= (1 << BLUE_LED); // blue OFF

    /* TODO Task 1 & 2: initialize buttons */
    DDRB &= ~(1 << BTN1_PIN);
    PORTB |= (1 << BTN1_PIN);

    DDRD &= ~(1 << BTN2_PIN);
    PORTD |= (1 << BTN2_PIN);


    /* TODO Task 3: initialize buzzer */
    DDRD |= (1 << BUZZ_PIN);
    PORTD &= ~(1 << BUZZ_PIN);

}

/* Brightness level variables. */
volatile uint8_t red_level = 0;
volatile uint8_t blue_level = 0;

static void interrupts_init(void)
{
    /* TODO Task 2: activate PB2 button interrupt on falling edge */
    EIMSK |= (1 << INT2);       // activeaza intreruperea INT2 (pinul PB2)
    EICRA |= (1 << ISC21);      // configureaza INT2 sa fie generata pe front


    /* TODO Task 2: activate PD6 button intterupt */
    PCICR |= (1 << PCIE3);      // activeaza intreruperile externe PCINT31..24 (portul D)
    PCMSK3 |= (1 << PCINT30);   // activeaza intreruperea corespunzatoare pinului PD6 (button)

}

/* TODO Task 2: implement PB2 button interrupt */

ISR (INT2_vect){    // s-a produs intreruperea aka s-a apasat butonul
    red_level = (red_level + 1) % LED_LEVELS;
  }


/* TODO Task 2: implement PD6 button interrupt */

ISR (PCINT3_vect){    // s-a produs intreruperea aka s-a apasat butonul
    blue_level = (blue_level + 1) % LED_LEVELS;
  }


#define TASK 2

int main()
{
    GPIO_init();
    USART0_init(CALC_USART_UBRR(PM_BAUD));
    USART0_use_stdio();

    /* Initialize timers */
    if (TASK == 3) {
        Timer0_init_ctc();
    } else {
        Timer0_init_pwm();
    }
    Timer1_init_systicks();
    Timer2_init_pwm();

    /* Initialize interrupts */
    interrupts_init();

    /* Activate global interrupts */
    sei();

    printf("It's timer hammer!\n");

    uint32_t last_ping = 0;
    uint32_t last_note = 0;
    uint16_t note_idx = 0;

    uint8_t animation = 0;

    for (;;) {
        /* Print alive message */
        if (SYSTICKS_PASSED(last_ping, 3000)) {
            last_ping = systicks;
            printf("Stayin' alive! [ticks=%ld]\n", systicks);
        }

        if (TASK == 1) {
            /* TODO Task 1: set animation mode */
             if (PINB & (1 << BTN1_PIN)) {
                animation = GLOW_UP;
            } else {
                animation = GLOW_DOWN;
            }
  

            /* TODO Task 1: set duty cycle for green LED
             * - calculate the current position in the animation cycle from systicks
             * - map the value from [0, LED_ANIMATION_TIME] to [0, 255]
             */
            if (animation == GLOW_UP) {
                OCR2A = (systicks % LED_ANIMATION_TIME) * 255 / LED_ANIMATION_TIME;
            } else {
                OCR2A = (LED_ANIMATION_TIME - systicks % LED_ANIMATION_TIME) * 255 / LED_ANIMATION_TIME;
            }

        } else if (TASK == 2) {
            /* TODO Task 2: set duty cycle for blue and red LEDs */
            OCR0A = blue_level * 255 / (LED_LEVELS - 1);
            OCR1B = red_level * OCR1A / (LED_LEVELS - 1);

        } else {
            /* TODO Task 3: play the music */

            if (SYSTICKS_PASSED(last_note, durations[note_idx])) {
                last_note = systicks;

                note_idx = (note_idx + 1) % num_notes;

                OCR0A = (F_CPU / 256) / surprise_notes[note_idx];
            }
        }
    }

    return 0;
}
