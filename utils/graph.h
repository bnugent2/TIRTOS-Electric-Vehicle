/*
 * graph_api.h
 *
 *  Created on: 30 May 2021
 *      Author: krisp
 */

#ifndef GRAPH_H_
#define GRAPH_H_

typedef struct Scale_Props{
    float Meas_Min;
    float Meas_Max;
    float Target_Min;
    float Target_Max;
} Scale_Props;

typedef struct Axis_Props{
    char *yLabel;
    char *xLabel;
    float yMax;
    float yMin;
    float xMax;
    float xMin;
} Axis_Props;

typedef struct Graph_Props{
    int gWidth;
    int gHeight;
    int xCoord;
    int yCoord;
} Graph_Props;

extern float scale_Y(float val, Scale_Props props);
extern void draw_graph(tContext *sContext, Graph_Props graph_props, Axis_Props axis_props);
extern void plot_point(tContext *sContext,float x, float y, Graph_Props graph_props, Axis_Props axis_props);


#endif /* GRAPH_H_ */

