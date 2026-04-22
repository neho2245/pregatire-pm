/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * PLEASE MODIFY THIS FILE * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "twi.h"

void twi_init(void) {
	/* Enable I2C power pin (PA6 - see chapter 5 in OCW) */
	// TODO 0: set PA6 as output with high level
     DDRA |= (1 << PA6);
     PORTA |= (1 << PA6);

	/* Reset I2C control register */
	TWCR = 0;
	
    /* Set I2C clock frequency */
    TWBR = (uint8_t)TWBR_VAL;
    // TODO 0: set bitrate in TWSR register (check twi.h to find prescaler value)
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
}

void twi_start(void) {    
    /* Enable I2C communication and clear interrupt flag */
    // TODO 0: send START condition (corresponding bit in TWCR)
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	/* Mandatory: wait for START condition to be sent */
	while (!(TWCR & (1 << TWINT)));
}

void twi_write(uint8_t data) {
    // Send a byte of data (TWCR + TWDR)
    TWDR = data;
    /* Enable I2C communication and clear interrupt flag */
    // TODO 1: set acknowledge bit (corresponding bit in TWCR)
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
    // TODO 1: wait for transfer to complete (TWINT flag)
    while (bit_is_clear (TWCR, TWINT))
    ;
}

void twi_read_ack(uint8_t *data) {
    /* Enable I2C communication and clear interrupt flag */
    // TODO 1: set acknowledge bit (corresponding bit in TWCR)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    // TODO 1: wait for transfer to complete (TWINT flag)
    while (bit_is_clear (TWCR, TWINT))
    *data = TWDR;
}

void twi_read_nack(uint8_t *data) {
    // TODO 1: read a byte of data with ACK disabled 
	// (same as above, but don't send acknowledge)
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (bit_is_clear (TWCR, TWINT))
    *data = TWDR;
}

void twi_stop(void) {
    /* Enable I2C communication and clear interrupt flag */
    // TODO 0: send STOP condition (corresponding bit in TWCR)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void twi_discover(void) {
    /* Search for I2C slaves */
    // HINT: An acknowledged SLA_R should enable a flag in TWSR. Check the datasheet (pg 275)!
    for (uint8_t i = 0x00; i < 0x7F; i++)  {
        twi_start();
		// TODO 2: write address (as seen in OCW hints)
        twi_write((i << 1) | 1);
        
        // TODO 2: check TWSR (see util/twi.h documentation for constants!)
        if (TWSR == TW_MR_SLA_ACK)
            printf("Device discovered on 0x%x\n", i);
    }
    twi_stop();
}
