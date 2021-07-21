/*
 * bmi105.c
 *
 *  Created on: 27 May 2021
 *      Author: bnuge
 */
#include <ti/drivers/I2C.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Slave Addresses */
#define BMI160_ADDRESS                  0x69

/* Register Addresses */
#define BMI160_REG_CMD                  0x7E

#define BMI160_REG_ACC_CONF             0x40
#define BMI160_REG_ACC_RANGE            0x41
#define BMI160_REG_TEST                 0x6D
#define BMI160_REG_DATA                 0x04
#define BMI160_REG_DATA_ACC_X_LSB       0x12
#define BMI160_REG_DATA_ACC_X_MSB       0x13
#define BMI160_REG_DATA_ACC_Y_LSB       0x14
#define BMI160_REG_DATA_ACC_Y_MSB       0x15
#define BMI160_REG_DATA_ACC_Z_LSB       0x16
#define BMI160_REG_DATA_ACC_Z_MSB       0x17


/* Byte Configs */
#define CMD_ACCEL_NORMAL_POWER          0x11
#define BMI160_TEST                     0x2C
#define BMI160_CONFIG                   0b00001001
#define BMI160_8G                       0b00001000
#define BMI160_4G                       0b00000101
#define BMI160_2G                       0b00000011
#define BMI160_TEST_ACTIVATE            0b01

bool writeI2CBMI160(uint8_t ui8Reg, uint8_t data, I2C_Handle i2c)
{
    uint8_t txBuffer[3];
    uint8_t rxBuffer[1];
    I2C_Transaction i2cTransaction;

    txBuffer[0] = ui8Reg;
    txBuffer[1] = data;
    i2cTransaction.slaveAddress = BMI160_ADDRESS;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;


    if (!I2C_transfer(i2c, &i2cTransaction))
    {
        return false;
    }

    return true;
}

bool readI2CBMI160(uint8_t* rxBuffX, uint8_t* rxBuffY, uint8_t* rxBuffZ, I2C_Handle i2c)
{
    uint8_t txBuffer[1];
    uint8_t rxBuffer[6];
    I2C_Transaction i2cTransaction;

    txBuffer[0] = BMI160_REG_DATA_ACC_X_LSB;
    i2cTransaction.slaveAddress = BMI160_ADDRESS;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 6;
    i2cTransaction.arg = "BMI160";

   // bool ret = I2C_transfer(i2c, &i2cTransaction);
    //if (!ret) {
        //System_printf("Unsuccessful I2C transfer");
       // System_flush();
    //}

    if (!I2C_transfer(i2c, &i2cTransaction))
    {
        System_abort("tran failed yo\n");
        return false;
    }

    rxBuffX[0] = rxBuffer[0];
    rxBuffX[1] = rxBuffer[1];
    rxBuffY[0] = rxBuffer[2];
    rxBuffY[1] = rxBuffer[3];
    rxBuffZ[0] = rxBuffer[4];
    rxBuffZ[1] = rxBuffer[5];

    return true;
}

void initBMI160(I2C_Handle i2c)
{
    uint8_t val = CMD_ACCEL_NORMAL_POWER;
    uint8_t val1 = BMI160_2G;
    uint8_t val2 = BMI160_CONFIG;

    writeI2CBMI160(BMI160_REG_CMD, val, i2c);
    writeI2CBMI160(BMI160_REG_ACC_CONF, val2, i2c);
    writeI2CBMI160(BMI160_REG_ACC_RANGE, val1, i2c);
}
