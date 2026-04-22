/**
 *  mpl3115a2.c - MPL3115A2 library for ATMega324P
 *  Datasheet: https://www.nxp.com/docs/en/data-sheet/MPL3115A2.pdf
 */

#include "mpl3115a2.h"
#include <util/delay.h>

void mpl3115a2_init(void)
{
    // Initialize MPL3115A2 with altimetry and wait for data to be ready
    twi_start();
    twi_write(MPL3115A2_ADDRESS << 1);
    twi_write(MPL3115A2_PT_DATA_CFG);
    twi_write(0x07);

    twi_start();
    twi_write(MPL3115A2_ADDRESS << 1);
    twi_write(MPL3115A2_CTRL_REG1);
    twi_write(0x39);

    twi_start();
    twi_write(MPL3115A2_ADDRESS << 1);
    twi_write(MPL3115A2_STATUS);

    twi_start();
    twi_write((MPL3115A2_ADDRESS << 1) | 0x01);

    // Wait for data ready.
    uint8_t sta = 0x00;
    while (!(sta & 0x08))
    {
        twi_read_ack(&sta);
    }
    twi_read_nack(&sta);

	// TODO 3.1: stop I2C transaction
    twi_stop();
}

void mpl3115a2_read_temperature(uint8_t *temperature)
{
    // TODO 3.2: Read the temperature from the sensor (steps detailed in OCW hints)

    // start I2C transaction
    twi_start();
    // put sensor's write address
    twi_write(MPL3115A2_ADDRESS << 1);
    // put temperature register address
    twi_write(MPL3115A2_T_MSB);

    // start I2C transaction
    twi_start();
    // put sensor's read address
    twi_write((MPL3115A2_ADDRESS << 1) | 1);

    // read temperature data
    twi_read_nack(temperature);

    // close I2C transaction
    twi_stop();
}
void mpl3115a2_read_pressure(uint32_t *pressure)
{
    // TODO 3.2: Read the pressure from the sensor

    // start I2C transaction
    twi_start();
    // put sensor's write address
    twi_write(MPL3115A2_ADDRESS << 1);
    // put pressure register address
    twi_write(MPL3115A2_P_MSB);

    // start I2C transaction
    twi_start();
    // put sensor's read address
    twi_write((MPL3115A2_ADDRESS << 1) | 1);

    // declare and read first byte of pressure data
    uint32_t pressure_msb = 0;
     twi_read_ack(&pressure_msb);
    // declare and read second byte of pressure data
    uint32_t pressure_csb = 0;
    twi_read_ack(&pressure_csb);
    // declare and read third byte of pressure data
    uint32_t pressure_lsb = 0;
    twi_read_nack(&pressure_lsb);

    // close I2C transaction
    twi_stop();

    // format the pressure data (be careful with the bit shifting)
    *pressure = (((pressure_msb << 12)) | ((pressure_csb << 4)) | (pressure_lsb >> 4)) >> 2;
}

