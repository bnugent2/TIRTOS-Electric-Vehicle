/*
 * graph_api.c
 *
 *  Created on: 30 May 2021
 *      Author: krisp
 */

/* std header files */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* grlib header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "drivers/frame.h"
#include "graph.h"



float scale_value(float val, Scale_Props props)
{
    float ret = ((val - props.Meas_Min)/(props.Meas_Max - props.Meas_Min))*(props.Target_Max - props.Target_Min) + props.Target_Min;
    return ret;
}



void draw_graph(tContext *sContext, Graph_Props graph_props, Axis_Props axis_props)
{

    tRectangle sRect;

    sRect.i16XMin = graph_props.xCoord;
    sRect.i16YMin = graph_props.yCoord;
    sRect.i16XMax = graph_props.gWidth + graph_props.xCoord;
    sRect.i16YMax = graph_props.gHeight + graph_props.yCoord;

    GrContextForegroundSet(sContext, ClrWhite);
    GrRectFill(sContext, &sRect);

    GrContextFontSet(sContext, &g_sFontCm12);
    GrContextForegroundSet(sContext, ClrWhite);
    // Draw X-axis Label
   // GrStringDraw(sContext, axis_props.xLabel,
                // strlen(axis_props.xLabel),
               //  sRect.i16XMax - GrStringWidthGet(sContext,axis_props.xLabel, strlen(axis_props.xLabel)),
                // sRect.i16YMax + GrStringHeightGet(sContext),
               //  false);
    // Draw Y-axis Label
   // GrStringDraw(sContext, axis_props.yLabel,
              //   strlen(axis_props.yLabel),
              //   graph_props.xCoord - GrStringWidthGet(sContext,axis_props.yLabel, strlen(axis_props.yLabel)),
               //  graph_props.yCoord - GrStringHeightGet(sContext),
              //   false);

    Scale_Props scaleY_props;
    scaleY_props.Meas_Max = axis_props.yMax;
    scaleY_props.Meas_Min = axis_props.yMin;
    scaleY_props.Target_Max = sRect.i16YMax;
    scaleY_props.Target_Min = sRect.i16YMin;

    char Y1[10];
    char Y2[10];
    char Y3[10];
    char Y4[10];
    char Y5[10];

    sprintf(Y1,"%0.0f",axis_props.yMin);
    sprintf(Y2,"%0.0f",axis_props.yMax * 0.25);
    sprintf(Y3,"%0.0f",axis_props.yMax * 0.5);
    sprintf(Y4,"%0.0f",axis_props.yMax * 0.75);
    sprintf(Y5,"%0.0f",axis_props.yMax);

    GrStringDraw(sContext,
                         Y1,
                         strlen(Y1),
                         sRect.i16XMin - GrStringWidthGet(sContext,Y1, strlen(Y1)),
                         sRect.i16YMax - GrStringHeightGet(sContext),
                         false);

    GrStringDrawCentered(sContext,
                         Y2,
                         strlen(Y2),
                         sRect.i16XMin - GrStringWidthGet(sContext,Y2, strlen(Y2))*0.5,
                         sRect.i16YMin + (sRect.i16YMax - scale_value(axis_props.yMax * 0.25, scaleY_props)),
                         false);

    GrStringDrawCentered(sContext,
                         Y3,
                         strlen(Y3),
                         sRect.i16XMin - GrStringWidthGet(sContext,Y3, strlen(Y3))*0.5,
                         sRect.i16YMin + (sRect.i16YMax - scale_value(axis_props.yMax * 0.5, scaleY_props)),
                         false);

    GrStringDrawCentered(sContext,
                         Y4,
                         strlen(Y4),
                         sRect.i16XMin - GrStringWidthGet(sContext,Y4, strlen(Y4))*0.5,
                         sRect.i16YMin + (sRect.i16YMax - scale_value(axis_props.yMax * 0.75, scaleY_props)),
                         false);

    GrStringDraw(sContext,
                 Y5,
                 strlen(Y5),
                 sRect.i16XMin - GrStringWidthGet(sContext,Y5, strlen(Y5)),
                 sRect.i16YMin + (sRect.i16YMax - scale_value(axis_props.yMax, scaleY_props)),
                 false);

    Scale_Props scaleX_props;
    scaleX_props.Meas_Max = axis_props.xMax;
    scaleX_props.Meas_Min = axis_props.xMin;
    scaleX_props.Target_Max = sRect.i16XMax;
    scaleX_props.Target_Min = sRect.i16XMin;

    int i;
    for(i = 0; i <= axis_props.xMax; i++)
    {
        char tmp[5];
        sprintf(tmp, "%0.0f",(float)i);
        GrStringDrawCentered(sContext,
                             tmp,
                             strlen(tmp),
                             scale_value(i, scaleX_props),
                             sRect.i16YMax + GrStringHeightGet(sContext) * 0.5,
                             false);
    }

}

void plot_point(tContext *sContext, float x, float y, Graph_Props graph_props, Axis_Props axis_props)
{
    tRectangle sRect;
    sRect.i16XMin = graph_props.xCoord;
    sRect.i16YMin = graph_props.yCoord;
    sRect.i16XMax = graph_props.gWidth + graph_props.xCoord;
    sRect.i16YMax = graph_props.gHeight + graph_props.yCoord;

    Scale_Props scaleX_props;
    scaleX_props.Meas_Max = axis_props.xMax;
    scaleX_props.Meas_Min = axis_props.xMin;
    scaleX_props.Target_Max = sRect.i16XMax;
    scaleX_props.Target_Min = sRect.i16XMin;

    Scale_Props scaleY_props;
    scaleY_props.Meas_Max = axis_props.yMax;
    scaleY_props.Meas_Min = axis_props.yMin;
    scaleY_props.Target_Max = sRect.i16YMax;
    scaleY_props.Target_Min = sRect.i16YMin;

    GrContextForegroundSet(sContext, ClrRed);
    GrCircleDraw(sContext,
                 scale_value(x, scaleX_props),
                 sRect.i16YMin + (sRect.i16YMax - scale_value(y, scaleY_props)),
                 2);

}


