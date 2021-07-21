/*
 * motor.h
 *
 *  Created on: 29 May 2021
 *      Author: bnuge
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define PULSES_PER_REV  24 // experimentally determined by rotating motor one rev

extern void Hall_Interupt(unsigned int index);
extern void inititalise_motor();
extern int getPulses();
extern void setPulses(int value);
extern int getDesiredrpm();
extern void setDesiredrpm(int value);
extern double PID(double input_rpm, double desired_rpm);
#endif /* MOTOR_H_ */
