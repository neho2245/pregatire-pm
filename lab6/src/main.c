/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * PLEASE MODIFY THIS FILE * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include <string.h>
#include <stdio.h>

#include <util/delay.h>
#include <avr/io.h>

#include "usart.h"
#include "mpl3115a2.h"
#include "twi.h"

#define PM_BAUD 9600

int main()
{
	// Initialize USART for writing to serial console.
	USART0_init(CALC_USART_UBRR(PM_BAUD));
	USART0_use_stdio();

	// Initialize I2C (including GPIO)
	twi_init();

	twi_discover();

	// TODO 3.1: initialize MPL3115A2 sensor.
	mpl3115a2_init();

	while (1)
	{
		// TODO 3.2: define variables for pressure and temperature
		uint32_t pressure;
		uint8_t temperature;

		// TODO 3.2: read and print temperature and pressure using the MPL3115A2 driver
		mpl3115a2_read_temperature(&temperature);
		mpl3115a2_read_pressure(&pressure);
		printf("Temperature: %hhu C\n", temperature);
		printf("Pressure: %lu Pa\n", pressure);
		
		_delay_ms(1000);
	}

	return 0;
}
