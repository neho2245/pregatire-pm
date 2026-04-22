#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "adc.h"
#include "st7735.h"
#include "snake.h"

#define BASE_TICK_MS     200
#define MIN_TICK_MS       80
#define SPEEDUP_PER_FOOD   5

#define BTN_ADC_CHANNEL   5  /* PA5 */

/* TODO/hint [task2]: define an array of ADC raw values for each button */
static const int btn_adc_values[] = { 510, 610, 680, 730, 768, 790 };
/* map each BTN to a direction (see snake enums) */
static const direction_t btn_to_directions[] = {
    DIR_NONE, /* note: don't map BTN1 */
    DIR_RIGHT, DIR_DOWN, DIR_DOWN /* why not? */,
    DIR_LEFT, DIR_UP,
};
/** Used to force re-pressessing */
static int prev_btn_state = 0;
/* It would be a good idea to do have your checks with +/- error margins */
#define BTN_ADC_ERR   15

// forward declarations
static direction_t read_btns(void);

/** Systick counter. */
volatile uint16_t ms_counter = 0;

ISR(TIMER1_COMPA_vect)
{
    ms_counter++;
}

/** Intiialize timer 1 to measure system ticks (1ms period) */
static void timer1_init(void)
{
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 187;
    TIMSK1 = (1 << OCIE1A);
}

/** Main game loop (blocking!) */
void task2(void)
{
    st7735_init();
    timer1_init();

    snake_game_t game;

    while (1)
    {
        snake_init(&game);
        snake_draw(&game);

        uint16_t tick_ms = BASE_TICK_MS;
        ms_counter = 0;

        while (!game.game_over)
        {
            /* low latency input: runs before the frame limiter! */
            direction_t d = read_btns();
            snake_set_dir(&game, d);

            /* frame limiter */
            if (ms_counter < tick_ms)
                continue;
            ms_counter = 0;

            snake_tick(&game);

            uint16_t speedup = (game.score / 10) * SPEEDUP_PER_FOOD;
            tick_ms = (speedup < BASE_TICK_MS - MIN_TICK_MS)
                            ? BASE_TICK_MS - speedup
                            : MIN_TICK_MS;
        }

        /** after game is over, wait for a button to be pressed */
        snake_game_over(game.score);
        _delay_ms(500);
        while (read_btns() == DIR_NONE)
            ;

        _delay_ms(200);
    }
}

/**
 * Reads the next button.
 * Note: called 
 */
static direction_t read_btns(void)
{
    int i;
    direction_t res_dir = DIR_NONE;

    uint16_t adc_raw;
    uint32_t adc_avg = 0;
    int adc_samples = 0;
    for (i=0; i<10; i++) {
        adc_raw = myAnalogRead(BTN_ADC_CHANNEL);
        /* discard first 2 values (filter) */
        if (i < 2) continue;
        adc_avg += adc_raw;
        adc_samples++;
    }
    adc_avg = adc_avg / 9;

    /* You can use printf() for experimenting ;) */
    /* printf("ADC value: %d\n", adc_raw); */
    /* _delay_ms(500); */

    int btn_idx = -1;
    for (i=0; i<6; i++) {
        /* translate ADC value to button index */
        /* don't forget to make the check with some error margins! */
        if ((adc_raw >= (btn_adc_values[i] - BTN_ADC_ERR)) && 
                (adc_raw <= (btn_adc_values[i] + BTN_ADC_ERR))) {
            btn_idx = i;
            break;
        }
    }
    if (btn_idx >= 0)
        res_dir = btn_to_directions[btn_idx];

    /* do not effect multiple times in a row if the same button is held! */
    int cur_state = res_dir;
    if (prev_btn_state == cur_state) {
        return DIR_NONE;
    }
    printf("BTN %d => %d\n", adc_raw, res_dir);
    prev_btn_state = cur_state;
    return res_dir;
}

