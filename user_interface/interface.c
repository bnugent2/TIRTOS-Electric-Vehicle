/*
 * interface.c
 *
 *  Created on: 1 Jun 2021
 *      Author: bnuge
 */

#include "interface.h"


#include <stdint.h>
#include <stdbool.h>
#include <xdc/runtime/System.h>

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

#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

#include "page1.h"
#include "page2.h"

int current_page = 0;

tCanvasWidget g_sPage1;
tCanvasWidget g_sPage2;

void onPageNext(tWidget *psWidget);
void onPageBack(tWidget *psWidget);


//Page Back
RectangularButton(g_sBack,0, 0, 0,
&g_sKentec320x240x16_SSD2119, 2, 2, 30, 30,
(PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT), ClrSkyBlue, ClrBlack,
 ClrGray, ClrWhite, g_psFontFixed6x8, "<", 0, 0, 125,
25, onPageBack);

//Next Page
RectangularButton(g_sNext,0, 0, 0,
&g_sKentec320x240x16_SSD2119, 35, 2, 30, 30,
(PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT), ClrSkyBlue, ClrBlack,
ClrGray, ClrWhite, g_psFontFixed6x8, ">", 0, 0, 125,
25, onPageNext);


void setup_ui(){
    g_sPage1 = get_Page1();
    g_sPage2 = get_Page2();
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBack);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPage1);

}

void onPageNext(tWidget *psWidget){

    if(current_page == 0){
        WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBack);
        WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
        WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPage2);
        WidgetPaint(WIDGET_ROOT);
        current_page = 1;
    }
}

void onPageBack(tWidget *psWidget){
        if(current_page == 1){
         //WidgetRemove((tWidget *)&g_sPage2);
        WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBack);
        WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
         WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPage1);
         WidgetPaint(WIDGET_ROOT);
         current_page = 0;
     }


}
