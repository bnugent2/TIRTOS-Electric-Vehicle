/*
 * sensors.h
 *
 *  Created on: 1 Jun 2021
 *      Author: bnuge
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <ti/drivers/I2C.h>


extern void init_sensors();
extern I2C_Handle intialiseI2CRuntime(I2C_Params i2cParams);
extern void readAccel(I2C_Handle i2c);


#endif /* SENSORS_H_ */
