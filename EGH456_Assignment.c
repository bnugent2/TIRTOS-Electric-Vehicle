/*
 *  ======== EGH456 Assignment.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/GPIO.h>
#include <xdc/runtime/Error.h>
#include <stdbool.h>
#include <xdc/runtime/Types.h>

 #include <ti/drivers/UART.h>


/* Board Header file */
#include "Board.h"
#include <stdint.h>
#include <stdbool.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>


/* Motor Driver Library */
#include "motorlib.h"
#include "motor.h"

/* Display & Graphics Header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/slider.h"
#include "images.h"

#include "interface.h"

#include "sensors.h"


#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

/* Utility Header Files*/
#include "utils/ustdlib.h"
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <time.h>

//==========================================//
//============* Globals * ==================//
//==========================================//
// Display Context
tContext sContext;

//Task 1 Params
#define TASKSTACKSIZE   512
Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

// Motor Task Params
Task_Struct motortaskStruct;
Char motortaskStack[TASKSTACKSIZE];

//Clock Handles
Clock_Struct motorClk;
Clock_Handle motorClkHandle;


Clock_Struct sensorClk;
Clock_Handle sensorClkHandle;

//I2C
I2C_Params i2cParamsRuntime;
I2C_Handle i2cHandleRuntime;

volatile int rpm = 0;
Event_Handle myEvent;
char  g_pcDateTimeBuf[40];

volatile bool g_bSetDate;


//==========================================//
//============== * Utilities * =============//
//==========================================//


//==========================================//
//========== * Display Widgets * ===========//
//==========================================//



extern void TouchScreenIntHandler(void);



//==========================================//
//========== * RTOS CLOCK FXN * ===========//
//==========================================//
Void motorClkFxn(UArg arg0)
{

    int numpulses = getPulses();
    int revolutions = numpulses/PULSES_PER_REV;
    rpm = revolutions * 120;

    double pwm_val = PID(rpm,getDesiredrpm());
    setDuty( pwm_val );

    setPulses(0);
}

Void sensorClkFxn(UArg arg0)
{
    //readAccel(i2cHandleRuntime);

}


//==========================================//
//============ * RTOS Tasks * ==============//
//==========================================//

// Paint and Update Display
void displayFxn(UArg arg0, UArg arg1)
{
    //Get frequency for display init //
        Types_FreqHz cpuFreq;
        BIOS_getCpuFreq(&cpuFreq);

    // Display Setup //
        Kentec320x240x16_SSD2119Init(&cpuFreq);
        GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
        GrContextFontSet(&sContext, &g_sFontCm20);


    //Init Touchscreen and callback function
        TouchScreenInit(&cpuFreq);
        TouchScreenCallbackSet(WidgetPointerMessage);

         //Add and paint the widgets to display

            setup_ui();
            WidgetPaint(WIDGET_ROOT);

            while(1)
            {
                WidgetMessageQueueProcess();

            }

}


/*
 *  ======== main ========
 */
int main(void){
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();


    /* Construct BIOS objects */
    Task_Params taskParams;

    //Task Threads
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.priority = 2;
    taskParams.instance->name = "display";
    Task_construct(&task1Struct, (Task_FuncPtr)displayFxn, &taskParams, NULL);

    // Clock Swis

    //RPM & PI Clock set to 0.5 Second
     Clock_Params motorClkParams;
     Clock_Params_init(&motorClkParams);
     motorClkParams.period = 500;
     motorClkParams.startFlag = TRUE;
     Clock_construct(&motorClk, (Clock_FuncPtr)motorClkFxn,500, &motorClkParams);

     //Sensor Clock set to 1 Second
      Clock_Params sensorClkParams;
      Clock_Params_init(&sensorClkParams);
      sensorClkParams.period = 1000;
      sensorClkParams.startFlag = TRUE;
      Clock_construct(&sensorClk, (Clock_FuncPtr)sensorClkFxn,1000, &sensorClkParams);

      //init_sensors();

      //System_printf("initialised Sensors\n");
     // System_flush();
      //i2cHandleRuntime = intialiseI2CRuntime(i2cParamsRuntime);
          //if (i2cHandleRuntime == NULL) {
         // System_abort("I2C create runtime failed\n");
         // }


    //Error Blocks
    Error_Block eb;
    Error_init(&eb);


    //Events
        myEvent = Event_create(NULL, &eb);
        if (myEvent == NULL) {
        System_abort("Event create failed");
        }

    //Motor
    inititalise_motor();


    System_printf("Starting Motor Driver Assignment\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();


    /* Start BIOS */
    BIOS_start();

    return (0);
}
