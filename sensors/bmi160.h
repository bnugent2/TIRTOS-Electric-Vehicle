/*
 * bmi105.h
 *
 *  Created on: 27 May 2021
 *      Author: bnuge
 */

#ifndef BMI105_H_
#define BMI105_H_


#include <ti/drivers/I2C.h>

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

extern void initBMI160(I2C_Handle i2c);
extern bool readI2CBMI160(uint8_t* rxBuffX, uint8_t* rxBuffY, uint8_t* rxBuffZ, I2C_Handle i2c);



#endif /* BMI105_H_ */
