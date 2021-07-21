/**************************************************************************************************
 *  Filename:       i2cOptDriver.c
 *  By:             Jesse Haviland
 *  Created:        1 February 2019
 *  Revised:        23 March 2019
 *  Revision:       2.0
 *
 *  Description:    i2c Driver for use with opt3001.c and the TI OP3001 Optical Sensor
 *************************************************************************************************/

// ----------------------- Includes -----------------------
#include "i2cOptDriver.h"
#include "inc/hw_memmap.h"

#include <xdc/runtime/System.h>
#include "Board.h"
#include <ti/drivers/I2C.h>

/*
 * Sets slave address to ui8Addr
 * Puts ui8Reg followed by two data bytes in *data and transfers
 * over i2c
 */
bool writeI2C(I2C_Handle handle, uint8_t ui8Addr, uint8_t regAddr, uint8_t *value)
{
    uint8_t             txBuffer[2];
    uint8_t             rxBuffer[1];
    I2C_Transaction     i2cTransaction;

    txBuffer[0] = regAddr;
    txBuffer[1] = value[0];
    //txBuffer[2] = value[1];

    i2cTransaction.slaveAddress = ui8Addr;

    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readCount = 0;


    if (!I2C_transfer(handle, &i2cTransaction)) {
        System_abort("Transaction Failed");
        return false;
    }

    return true;
}

/*
 * Sets slave address to ui8Addr
 * Writes ui8Reg over i2c to specify register being read from
 * Reads three bytes from i2c slave. The third is redundant but
 * helps to flush the i2c register
 * Stores first two received bytes into *data
 */
bool readI2C(I2C_Handle handle, uint8_t ui8Addr, uint16_t regAddr,
             uint8_t *data)

{
    uint8_t txBuffer[4];
    uint8_t rxBuffer[2];

    I2C_Transaction i2cTransaction;
    txBuffer[0] = regAddr;


    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readCount = 2;

    if (!I2C_transfer(handle, &i2cTransaction))
    {
        // GPIO_write(Board_LED1, Board_LED_ON);
        //System_abort("Bad I2C transfer!");
        return false;
    }
    data[0] = rxBuffer[0];
    data[1] = rxBuffer[1];
    return true;
}

