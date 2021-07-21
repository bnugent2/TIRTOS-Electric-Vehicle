/*
 * motor.c
 *
 *  Created on: 29 May 2021
 *      Author: bnuge
 */

# include "motor.h"
# include "motorlib.h"
# include <ti/drivers/GPIO.h>
# include "board.h"
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>


int pulses = 0;
int desiredrpm = 700;
// Setup GPIO callbacks and setup motor
void inititalise_motor(){

    Error_Block motor_eb;
    Error_init(&motor_eb);

    initMotorLib(90,&motor_eb);

     if(Error_check(&motor_eb)) {
        System_abort("Motor init create failed");
        }

    //enable HALL Interupts
    GPIO_setCallback(HALL_A, Hall_Interupt);
    GPIO_enableInt(HALL_A);
    GPIO_setCallback(HALL_B, Hall_Interupt);
    GPIO_enableInt(HALL_B);
    GPIO_setCallback(HALL_C, Hall_Interupt);
    GPIO_enableInt(HALL_C);

    enableMotor();
    setDuty(20); // set initial duty

    //Manually start motor
    Hall_Interupt(4);

}

int getPulses(){

    return pulses;
}

void setPulses (int value){
    pulses = value;
}

int getDesiredrpm(){

    return desiredrpm;
}

void setDesiredrpm (int value){
    desiredrpm = value;
}


double PID(double input_rpm, double desired_rpm){

    double error;
    double output;

    double kp = 0.009;
    double ki = 0.002;

    error = desired_rpm - input_rpm;
    double intError = intError + error;

    output = kp*error + ki*intError;

    return output;
}

// GPIO Callback function to update motor
void Hall_Interupt(unsigned int index)
{
    int Hall_A_read = GPIO_read(HALL_A);
    int Hall_B_read = GPIO_read(HALL_B);
    int Hall_C_read = GPIO_read(HALL_C);
    pulses++;


    updateMotor(Hall_A_read,Hall_B_read,Hall_C_read);
}


