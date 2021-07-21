/*
 * sensors.c
 *
 *  Created on: 1 Jun 2021
 *      Author: bnuge
 */
#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>
#include "bmi160.h"
#include <string.h>

I2C_Params i2cParamsSetup;
uint16_t AccX;
uint16_t AccY;
uint16_t AccZ;

void I2C_Callback(I2C_Handle handle, I2C_Transaction *transaction, bool success){

if (!success)
{
    System_printf("transaction failed\n");
    return;
}

I2C_Transaction result = *transaction;
char * arg = (char *) result.arg;
if(strcmp(arg, "BMI160")==0){


   // (uint8_t*)&AccX[0] = result->readBuf[0];
    //(uint8_t*)&AccX[1] = result->readBuf[1];
    // (uint8_t*)&AccY[0] = result->readBuf[2];
    // (uint8_t*)&AccY[1] = result->readBuf[3];
   // (uint8_t*)&AccZ[0] = result->readBuf[4];
   // (uint8_t*)&AccZ[1] = result->readBuf[5];

return;
}

if(strcmp(arg, "OPT3001")==0){
    System_printf("OPT3001 read\n");

}
System_printf("something else\n");
     System_flush();

}

I2C_Handle intialiseI2CSetup(I2C_Params i2cParams)
{
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.transferCallbackFxn = NULL;
    I2C_Handle i2c = I2C_open(0, &i2cParams);
    if (i2c == NULL)
    {
        return NULL;
    }
    else
    {
        return i2c;
    }
}


I2C_Handle intialiseI2CRuntime(I2C_Params i2cParams)
{
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.transferCallbackFxn = NULL; //I2C_Callback
    I2C_Handle i2c = I2C_open(0, &i2cParams);
    if (i2c == NULL)
    {
        return NULL;
    }
    else
    {
        return i2c;
    }
}

void init_sensors()
{
    //Open I2C connection
    I2C_Handle i2cHandleSetup = intialiseI2CSetup(i2cParamsSetup);

    if (i2cHandleSetup == NULL) {
                  System_abort("I2C create setup failed\n");
                  }
    // BMI160 Accelerometer
    initBMI160(i2cHandleSetup);
    System_printf("done!\n");
    System_flush();
    // Light Sensor
    //done work close this handle
    I2C_close(i2cHandleSetup);

}

 void readAccel(I2C_Handle i2c){
     readI2CBMI160((uint8_t*)&AccX, (uint8_t*)&AccY, (uint8_t*)&AccZ, i2c);
 }

int AverageFilter(int buffer[], int size)
{

    int total = 0;        // the running total
    int average = 0;        // the average
    int i;
    //total = total - readings[readIndex];
    // add the reading to the total:
    for (i = 0; i < size; i++)
    {
        total = total + buffer[i];
    }
    // calculate the average:
    average = total / size;

    return average;

}
