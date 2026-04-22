/**
 *  mpl3115a2.h - MPL3115A2 library for ATMega324P
 *  Datasheet: https://www.nxp.com/docs/en/data-sheet/MPL3115A2.pdf
*/

#include "twi.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include "usart.h"

/* MPL3115A2 slave address */
#define MPL3115A2_ADDRESS 0x60

/* MPL3115A2 registers */
#define MPL3115A2_STATUS      0x00 // Sensor Status Register
#define MPL3115A2_P_MSB       0x01 // Pressure Data Out MSB
#define MPL3115A2_P_CSB       0x02 // Pressure Data Out CSB
#define MPL3115A2_P_LSB       0x03 // Pressure Data Out LSB
#define MPL3115A2_T_MSB       0x04 // Temperature Data Out MSB
#define MPL3115A2_T_LSB       0x05 // Temperature Data Out LSB
#define MPL3115A2_WHO_AM_I    0x0C // Device Identification Register
#define MPL3115A2_PT_DATA_CFG 0x13 // PT Data Configuration register
#define MPL3115A2_CTRL_REG1   0x26 // Control Register 1
#define MPL3115A2_CTRL_REG2   0x27 // Control Register 2
#define MPL3115A2_CTRL_REG3   0x28 // Control Register 3

/**
 * \brief Initialize MPL3115A2 for reading data with altimetry
*/
void mpl3115a2_init(void);

/**
 * \brief Read the pressure value in Pa
*/
void mpl3115a2_read_pressure(uint32_t *pressure);

/**
 * \brief Read temperature value in Celsius
*/
void mpl3115a2_read_temperature(uint8_t *temperature);
