/*
 * page2.c
 *
 *  Created on: 1 Jun 2021
 *      Author: bnuge
 */


#include "page2.h"



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

#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

#include "graph.h"



tContext sContext;

extern tCanvasWidget g_sPage2Background;
void onPaint (tWidget *psWidget, tContext *psContext);


//Base Canvas
//Base Canvas
Canvas(g_sPage2Background, WIDGET_ROOT,0,0,
       &g_sKentec320x240x16_SSD2119, 0, 0, 320, 240,
       CANVAS_STYLE_FILL, ClrBlue, 0, 0, 0, 0, 0, 0);


void onPaint (tWidget *psWidget, tContext *psContext){

    Axis_Props g_sAxis;

    g_sAxis.xLabel = "Time";
    g_sAxis.yLabel = "RPM";
    g_sAxis.xMax = 10;
    g_sAxis.yMax = 4000;
    g_sAxis.xMin = 0;
    g_sAxis.yMin = 0;

    Graph_Props g_sGraph;
    g_sGraph.gWidth = 240;
    g_sGraph.gHeight = 160;
    g_sGraph.yCoord = 50;
    g_sGraph.xCoord = 30;

   // draw_graph(psContext,g_sGraph,g_sAxis);
}

//==========================================//
//======= * Getters * =======//
//==========================================//

tCanvasWidget get_Page2 (){

    return g_sPage2Background;
}
