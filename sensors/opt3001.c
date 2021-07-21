/**************************************************************************************************
 *  Filename:       opt3001.c
 *  Revised:
 *  Revision:
 *
 *  Description:    Driver for the Texas Instruments OP3001 Optical Sensor
 *
 *  Copyright (C) 2014 - 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "i2cOptDriver.h"
#include "opt3001.h"
#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

/* Slave address */
#define OPT3001_I2C_ADDRESS             0x47

/* Register addresses */
#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOW_LIMIT                   0x02
#define REG_HIGH_LIMIT                  0x03

#define REG_MANUFACTURER_ID             0x7E
#define REG_DEVICE_ID                   0x7F

/* Register values */
#define MANUFACTURER_ID                 0x5449  // TI
#define DEVICE_ID                       0x3001  // Opt 3001
#define CONFIG_RESET                    0xC810
#define CONFIG_TEST                     0xCC10
#define CONFIG_ENABLE                   0x10C4 //0x10C4 // 0xC410   - 100 ms, continuous
#define CONFIG_DISABLE                  0x10C0 // 0xC010   - 100 ms, shutdown

#define LOW_LIMIT                       0xFF0F  // 0000
#define HIGH_LIMIT                      0xFF6F   //0110

/* Bit values */
#define DATA_RDY_BIT                    0x0080  // Data ready

/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_LENGTH                     2

/* ------------------------------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Public functions
 * -------------------------------------------------------------------------------------------------
 */


/**************************************************************************************************
 * @fn          sensorOpt3001Init
 *
 * @brief       Initialize the temperature sensor driver
 *
 * @return      none
 **************************************************************************************************/
bool sensorOpt3001Init(I2C_Handle i2c)
{
    sensorOpt3001Enable(i2c,false);

    return (true);
}


/**************************************************************************************************
 * @fn          sensorOpt3001Enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Enable(I2C_Handle i2c,bool enable)
{
    uint16_t val;

    if (enable)
    {
        val = CONFIG_ENABLE;
    }
    else
    {
        val = CONFIG_DISABLE;
    }

    writeI2C(i2c,OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t*)&val);
}


/**************************************************************************************************
 * @fn          sensorOpt3001Read
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      TRUE if valid data
 **************************************************************************************************/
bool sensorOpt3001Read(I2C_Handle i2c,uint16_t *rawData)
{
    bool success;
    uint16_t val;

    success = readI2C(i2c,OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)&val);


    if (success)
    {
        success = ((val>>8 &0xFF) & DATA_RDY_BIT) == DATA_RDY_BIT;
    }

    if (success)
    {
        success = readI2C(i2c,OPT3001_I2C_ADDRESS, REG_RESULT, (uint8_t *)&val);
    }

    if (success)
    {

        *rawData = (val << 8) | (val>>8 &0xFF);
    }
    else
    {

    }

    return (success);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 **************************************************************************************************/
bool sensorOpt3001Test(I2C_Handle handle)
{
    uint16_t val = 0;
    // Check manufacturer ID
    readI2C(handle, OPT3001_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val);
    //readI2C(OPT3001_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != MANUFACTURER_ID)
    {
        System_printf("Manufacturer ID incorrect: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);
        return (false);
    }

    System_printf("Manufacturer ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    // Check device ID
    readI2C(handle, OPT3001_I2C_ADDRESS, REG_DEVICE_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != DEVICE_ID)
    {
        return (false);
    }

    System_printf("Device ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    return (true);
}

/**************************************************************************************************
 * @fn          sensorOpt3001Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawData - raw data from sensor
 *
 * @param       convertedLux - converted value (lux)
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001Convert(uint16_t rawData, float *convertedLux)
{
    uint16_t e, m;

    m = rawData & 0x0FFF;
    e = (rawData & 0xF000) >> 12;

    *convertedLux = m * (0.01 * exp2(e));
}


/**************************************************************************************************
 * @fn          sensorOpt3001HighLimit
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 **************************************************************************************************/
void sensorOpt3001SetLimit(I2C_Handle i2c)
{
    uint16_t val;

    val = LOW_LIMIT;

    writeI2C(i2c,OPT3001_I2C_ADDRESS, REG_LOW_LIMIT, (uint8_t*)&val);

    val = HIGH_LIMIT;

    writeI2C(i2c,OPT3001_I2C_ADDRESS, REG_HIGH_LIMIT, (uint8_t*)&val);

    //uint16_t value;

        // Check manufacturer ID
        //readI2C(i2c,OPT3001_I2C_ADDRESS, REG_LOW_LIMIT, (uint8_t *)&value);
       // value = (value << 8) | (value>>8 &0xFF);

        //UARTprintf("Low Limit Register Reading: %u%u\n", value & 0x00FF, (value >> 8) & 0x00FF);


        //uint16_t value1;

              // Check manufacturer ID
              //readI2C(i2c,OPT3001_I2C_ADDRESS, REG_HIGH_LIMIT, (uint8_t *)&value1);

              //UARTprintf("High Limit Register Reading: %u%u\n", value1 & 0x00FF, (value1 >> 8) & 0x00FF);


}
