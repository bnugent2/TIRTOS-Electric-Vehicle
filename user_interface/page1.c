/*
 * UI.c
 *
 *  Created on: 1 Jun 2021
 *      Author: bnuge
 */

#include "page1.h"


#include <stdint.h>
#include <stdbool.h>

/* Display & Graphics Header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/slider.h"
#include "images.h"

#include "motorlib.h"
#include "motor.h"
#include "utils/ustdlib.h"

#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

tContext sContext;

static char motorSpeed[22] = "700 RPM";
int userSetRPM;

//==========================================//
// Forward reference widget structs.
//==========================================//
extern tCanvasWidget g_sPage1Background;
extern tCanvasWidget g_sBackground2;
extern tContainerWidget g_sContainer2;
extern tContainerWidget g_sContainer_Light;
extern tContainerWidget g_sContainer_Temp;
extern tCanvasWidget g_sMSpeed;
extern tCanvasWidget g_sSliderValueCanvas;

//==========================================//
// Forward reference for input handlers.
//==========================================//
void onPageSwitch(tWidget *psWidget);
void onPageNext(tWidget *psWidget);
void onMotorStart(tWidget *psWidget);
void onMotorStop(tWidget *psWidget);
void onPageBack(tWidget *psWidget);
void onFirstPaint(tWidget *psWidget, tContext *psContext);
void onMSpeedPaint (tWidget *psWidget, tContext *psContext);
void onSliderChange(tWidget *psWidget, int32_t i32Value);
void onSpeedSet(tWidget *psWidget);

//==========================================//
// Page 1 Widgets
//==========================================//
//Motor Speed Slider
tSliderWidget g_psSlider_RPM = SliderStruct(&g_sContainer2, 0, 0,
                 &g_sKentec320x240x16_SSD2119, 5, 115, 160, 30, 0, 100, 25,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                  SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
                 ClrSkyBlue, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
                 &g_sFontCm20, motorSpeed, 0, 0, onSliderChange);
//Light Indicator
tCanvasWidget g_psLight_Indicator = CanvasStruct(&g_sContainer_Light, 0, 0,
                 &g_sKentec320x240x16_SSD2119, 220, 45, 80, 80,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pui8LightOn, 0);


tPushButtonWidget g_psPushButtons[] =
{

//Motor On
RectangularButtonStruct(&g_sContainer2, g_psPushButtons + 1, 0,
 &g_sKentec320x240x16_SSD2119, 70, 60, 60, 30,
 (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
  PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
  ClrGray, ClrWhite, &g_sFontCm18, "ON", 0, 0, 125,
25, onMotorStart),
//Motor Off
RectangularButtonStruct(&g_sContainer2, g_psPushButtons + 2, 0,
 &g_sKentec320x240x16_SSD2119, 135, 60, 60, 30,
 (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
  PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
  ClrGray, ClrWhite, &g_sFontCm18, "OFF", 0, 0, 125,
25, onMotorStop),
//E-Stop Down
RectangularButtonStruct(&g_sContainer2, g_psPushButtons + 3, 0,
 &g_sKentec320x240x16_SSD2119, 5, 180, 60, 30,
 (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
  PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
  ClrGray, ClrWhite, g_psFontFixed6x8, "<", 0, 0, 125,
25, onPageSwitch),
//E-Stop Up
RectangularButtonStruct(&g_sContainer2, g_psPushButtons + 4, 0,
 &g_sKentec320x240x16_SSD2119, 135, 180, 60, 30,
 (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
  PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
  ClrGray, ClrWhite, g_psFontFixed6x8, ">", 0, 0, 125,
25, onPageSwitch),
//Confirm Speed
RectangularButtonStruct(&g_sContainer2, &g_psSlider_RPM, &g_sMSpeed,
 &g_sKentec320x240x16_SSD2119, 168, 115, 32, 30,
 (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
  PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
  ClrGray, ClrWhite, &g_sFontCm18, "OK", 0, 0, 125,
25, onSpeedSet)
};

//Temperature Limit Buttons
tPushButtonWidget g_psTempButtons[] =
{
 //Temp Limit Down
 RectangularButtonStruct(&g_sContainer_Temp, g_psTempButtons + 1, 0,
  &g_sKentec320x240x16_SSD2119, 215, 140, 40, 30,
  (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
   PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
   ClrGray, ClrWhite, g_psFontFixed6x8, "^", 0, 0, 125,
 25, onPageSwitch),
 //Temp limit Up
 RectangularButtonStruct(&g_sContainer_Temp, 0, 0,
  &g_sKentec320x240x16_SSD2119, 215, 180,40, 30,
  (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
   PB_STYLE_AUTO_REPEAT), ClrSkyBlue, ClrBlack,
   ClrGray, ClrWhite, g_psFontFixed6x8, "v", 0, 0, 125,
 25, onPageSwitch)
};


//Motor Container
Container(g_sContainer2, &g_sPage1Background, &g_sContainer_Light, &g_psPushButtons,
          &g_sKentec320x240x16_SSD2119, 2, 38, 200, 180,
          (CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT | CANVAS_STYLE_APP_DRAWN |
           CTR_STYLE_TEXT_CENTER), ClrGray, ClrWhite, ClrWhite,
          &g_sFontCm18, "Motor Control");

//Lights Container
Container(g_sContainer_Light, &g_sPage1Background, &g_sContainer_Temp, &g_psLight_Indicator,
          &g_sKentec320x240x16_SSD2119, 210, 38, 100, 80,
          (CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT |
           CTR_STYLE_TEXT_CENTER), ClrGray, ClrWhite, ClrWhite,
          &g_sFontCm18, "Lights");

//Temperature Container
Container(g_sContainer_Temp, &g_sPage1Background,0, &g_psTempButtons,
          &g_sKentec320x240x16_SSD2119, 210, 120, 100, 100,
          (CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT |
           CTR_STYLE_TEXT_CENTER), ClrGray, ClrWhite, ClrWhite,
          &g_sFontCm18, "Accel");



//Canvas For Motor Text
Canvas(g_sMSpeed, g_psPushButtons + 4, 0,0 ,
       &g_sKentec320x240x16_SSD2119, 0, 0, 320, 240,
       CANVAS_STYLE_APP_DRAWN, ClrWhite, 0, 0, 0, 0, 0, onMSpeedPaint);

//Base Canvas
Canvas(g_sPage1Background, WIDGET_ROOT,0,&g_sContainer2 ,
       &g_sKentec320x240x16_SSD2119, 0, 0, 320, 240,
       CANVAS_STYLE_APP_DRAWN, ClrBlack, 0, 0, 0, 0, 0, onFirstPaint);

Canvas(g_sSliderValueCanvas, &g_sContainer2, &g_psSlider_RPM, 0,
       &g_sKentec320x240x16_SSD2119, 210, 50, 10,2,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, ClrBlack, 0, ClrSilver,
       &g_sFontCm24, "RPM",
       0, 0);



//==========================================//
//======= * Display Paint Functions * ======//
//==========================================//

// Write strings to the first Canvas
void onFirstPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, &g_sFontCm18);
    GrContextForegroundSet(psContext, ClrWhite);
    //GrStringDraw(psContext,g_pcDateTimeBuf, sizeof(g_pcDateTimeBuf),100, 10, 0);
    GrStringDraw(psContext, " 31 MAY 2021 05:34:05PM", 23, 140, 2, 0);

}

void onMSpeedPaint (tWidget *psWidget, tContext *psContext){
        GrContextFontSet(psContext, &g_sFontCm18);
        GrContextForegroundSet(psContext, ClrWhite);
        GrStringDraw(psContext, "E-Stop :",8, 10, 155, 0);
        GrStringDraw(psContext, "Speed :",7, 10, 88, 0);
}
//==========================================//
//======= * Display Input Handlers * =======//
//==========================================//



void onPageSwitch(tWidget *psWidget){
    WidgetRemove((tWidget *)&g_psSlider_RPM);
    WidgetPaint(WIDGET_ROOT);
}

void onSpeedSet(tWidget *psWidget){
    setDesiredrpm(userSetRPM);
}

void onMotorStart(tWidget *psWidget){
    enableMotor();
}

void onMotorStop(tWidget *psWidget){
    disableMotor();
}

void onSliderChange(tWidget *psWidget, int32_t i32Value){

   int OldRange = (100 - 0);
   int NewRange = (5000 - 0);
   userSetRPM = (((i32Value - 0) * NewRange) / OldRange) + 0;

    usprintf(motorSpeed, "%d rpm",userSetRPM );
    WidgetPaint((tWidget *)&g_psSlider_RPM);
}


//==========================================//
//======= * Getters * =======//
//==========================================//

tCanvasWidget get_Page1 (){

    return g_sPage1Background;
}

