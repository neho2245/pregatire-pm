#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

#define SS PB4

void SPI_init(){

	/* TODO1: set MOSI and SCK output, all others input */
	DDRB |= (1<<SPI_MOSI) | (1<<SPI_SCK);

	/* TODO1: set SS PIN as output and set it on HIGH */
	DDRB |= (1<<SS);
	PORTB |= (1<<SS);


	/* TODO1: enable SPI, set it as Master, set clock rate at fosc/16 */
	SPCR0 |= (1<<SPE0) | (1<<MSTR0) | (1<<SPR00);
}

uint8_t SPI_exchange(uint8_t data){
	// TODO1: send a byte of data to the slave and return the response byte received from him in this transmission
	SPDR0 = data;
	while(bit_is_clear(SPSR0, SPIF0))
	;
	return SPDR0;
}

