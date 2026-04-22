#include <avr/io.h>

#include "adc.h"

/* TODO task 0: implement ADC init! */
void adc_init(void)
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);
}

/* TODO task 0: implement ADC read! */
uint16_t myAnalogRead(uint8_t channel)
{
    // Force input channel to be between 0 and 7 (as ADC pins are PA0-7)
    channel &= 0b00000111;

    // 1. Clear the old channel value (if any, last 5 bits in ADMUX)
    ADMUX &= 0b11100000;
    // 2. Select the new channel in ADMUX
    ADMUX |= channel;
    // 3. Start single conversion
    ADCSRA |= (1 << ADSC);
    // 4. Busy wait for conversion to complete
    while (ADCSRA & (1 << ADSC));

    // Return ADC value
    return (ADC);
}
