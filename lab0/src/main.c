#include <avr/io.h>
#include <util/delay.h>

#define GPIO_SET_PD(pin, state)  PORTD = PORTD & ((~_BV(pin)) | (state ? _BV(pin) : 0))
#define GPIO_SET_PB(pin, state)  PORTB = PORTB & ((~_BV(pin)) | (state ? _BV(pin) : 0))

// apply inverted logic here
#define GPIO_SET_RED(state)  GPIO_SET_PD(PD5, !state)
#define GPIO_SET_GREEN(state)  GPIO_SET_PD(PD7, !state)
#define GPIO_SET_BLUE(state)  GPIO_SET_PB(PB3, !state)
// returns true if the button IS PRESSED, false (0) otherwise
#define BTN_PRESSED()  ((PINB & _BV(PB2)) == 0)

// Semaphore FSM states
typedef enum {
    SEM_RED = 0,
    SEM_YELLOW,
    SEM_GREEN,
    SEM_GREEN_BLINK
} SemaphoreState;

SemaphoreState state = SEM_RED;
uint16_t loop_ticks = 0;
uint16_t last_btn_check_ticks = 0;
uint16_t button_press_ticks = 0;
uint16_t yellow_started_ticks = 0;
uint16_t green_started_ticks = 0;
uint8_t green_blink_state = 0;
uint16_t green_blink_ticks = 0;


int main() {
    // set LEDs as output
    DDRD |= _BV(PD5) | _BV(PD7);  // R, G
    DDRB |= _BV(PB3);             // B

    // PB2 button is input + pullup enabled 
    DDRB &= ~_BV(PB2);
    PORTB |= _BV(PB2);

    // keep blue led on at startup for 2s, turn off the rest
    // (note: our macros above already use inverted logic!)
    GPIO_SET_BLUE(1);
    GPIO_SET_RED(0);
    GPIO_SET_GREEN(0);
    _delay_ms(2000);

    // PLAYTIME IS OVER, time for tasks 2+3!
    GPIO_SET_BLUE(0);
    GPIO_SET_RED(1);

    // note: loop_ticks measure approx. ~10ms, incremented & delayed at the end of the loop
    while(1) {
        // efficiently debounce button checks every 10ms
        if (last_btn_check_ticks != loop_ticks) {
            last_btn_check_ticks = loop_ticks; // yep, the loop increments by 1 => 10ms passed!
            if (BTN_PRESSED()) {
                // count the ticks while button is pressed
                button_press_ticks++;
                // long press detection (200 ticks * 10ms = 2 seconds)
                if (button_press_ticks >= 200) {
                    // reset state
                    state = SEM_RED;
                    button_press_ticks = 0; // prevent repeated action
                }
            } else {
                // debouncing (filter glitches in the matrix)
                if (button_press_ticks > 5) {
                    // we have a short press! switch the semaphore:
                    if (state == SEM_RED) {
                        state = SEM_YELLOW;
                        yellow_started_ticks = loop_ticks;
                    }
                }
                button_press_ticks = 0; // prevent repeated action
            }
        }

        // main semaphore LED state machine
        // warning: no sleeps allowed!! this will execute almost every 10ms!
        switch(state) {
            case SEM_RED:
                GPIO_SET_RED(1);
                GPIO_SET_GREEN(0);
                break;

            case SEM_YELLOW:
                // Yellow = Red + Green
                GPIO_SET_RED(1);
                GPIO_SET_GREEN(1);
                // after 200 ticks ~= 2 seconds => yellow state is over
                if ((loop_ticks - yellow_started_ticks) > 200) {
                    // go to green!
                    state = SEM_GREEN;
                    green_started_ticks = loop_ticks;
                }
                break;

            case SEM_GREEN:
                GPIO_SET_RED(0);
                GPIO_SET_GREEN(1);
                // after 1000 ticks ~= 10 seconds => go to green blinking state
                if ((loop_ticks - green_started_ticks) > 1000) {
                    green_blink_ticks = 0;
                    state = SEM_GREEN_BLINK;
                }
                break;

            case SEM_GREEN_BLINK:
                GPIO_SET_RED(0);
                // this count the time until next blink
                green_blink_ticks++;
                if (green_blink_ticks > 30) { // e.g., blink every 300ms
                    green_blink_state = !green_blink_state;
                    GPIO_SET_GREEN(green_blink_state);
                    green_blink_ticks = 0; // reset blinking counter to 0
                }
                // after 10 + 2 seconds after the green state started, we end this #$%^
                if ((loop_ticks - green_started_ticks) > 1200) {
                    green_started_ticks = 0;
                    state = SEM_RED;
                }
                break;
        }

        // here's the main tick counter logic (EZ)
        loop_ticks++;
        _delay_ms(10);
    }
    
    return 0;
}

