#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "adc.h"
#include "task3.h"

// Sensor range: -10°C to 125°C mapped to ADC 0–1023
#define TEMP_LOW   (-10.0f)
#define TEMP_HIGH  (125.0f)
#define ADC_LOW    0
#define ADC_HIGH   1023

// Temperature threshold in °C. Trigger alarm if exceeded.
#define TEMP_THRESHOLD 26.0f


void printf_teleplot(const char *name, float value);

/**
 * Remaps (translates + rescales) an ADC value to a floating-point conversion.
 * > old_low – old_high: ADC range  (0 – 1023)
 * > new_low – new_high: New value conversion range (e.g., -10.0 – 125.0 for temperatures)
 *
 * Note: float is slow on AVR, but since we will have small temperature changes
 * and sample every second, we can compromise here.
 */
float remap_interval(int16_t old_low, int16_t old_high,
                     float new_low, float new_high,
                     int16_t value)
{
    return new_low + (float)(value - old_low) * (new_high - new_low)
                   / (float)(old_high - old_low);
}

/** Drives the speaker (blocking) */
void alarm(void)
{
    int delay = 1;
    int i;
    DDRD |= (1 << PD4);
    for (i = 0; i < 50 / delay; i++)
    {
        PIND = (1 << PD4);
        _delay_ms(1);
    }
}

/*
 * TODO [task3]: ADC interrupt
 *
 * Read the ADC result, convert it to temperature using remap_interval, and
 * trigger the alarm if the temperature exceeds the threshold.
 */
ISR(ADC_vect)
{
    // Clear the timer1 interrupt flag by writing a 1 to it
    // Since we don't check TIMER1_COMPB_vect (which set clear OCF1B),
    // we need to set OCF1B manually to trigger another interrupt.
    // you can get the same result by setting an empty ISR for TIMER1_COMPB_vect.
    TIFR1 |= (1 << OCF1B);

    uint16_t adc_value = ADC;

    float temperature = remap_interval(ADC_LOW, ADC_HIGH,
                                       TEMP_LOW, TEMP_HIGH,
                                       adc_value);

    if (temperature > TEMP_THRESHOLD)
        alarm();

    printf_teleplot("Temperature", temperature);
}

/**
 * TODO [task3]: Configure ADC for auto-triggering from Timer1
 * HINT: The ADC trigger source is selected via the ADTS[2:0] bits in ADCSRB.
 */
void adc_auto_trigger_init(int channel)
{
    // Set the new channel
    ADMUX &= 0b11100000;
    ADMUX |= channel;

    // Enable ADC
    ADCSRA |= (1 << ADEN);
    // Enable ADC interrupts and auto-triggering
    ADCSRA |= (1 << ADIE) | (1 << ADATE);
    // Set auto trigger source
    ADCSRB = (1 << ADTS2) | (1 << ADTS0);  // Trigger source: Timer1 Compare Match B
}

void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12);
    OCR1A = 46874;
    // Now why did we set up the second comparator?
    OCR1B = 46874;
}

/** Prints a teleplot-formatted string */
void printf_teleplot(const char *name, float value)
{
    printf(">%s:%.2f\n", name, value);
}

/** Task 3: enable continuous ADC sampling using interrupts! */
void task3(void)
{
    timer1_init();
    adc_auto_trigger_init(0);
    while (1);
}
