#ifndef DISP_H
#define DISP_H

#include "initgl.h"
#include "cube.h"

void DIS_DrawLine(windowContext *winParam, float *origin, float *end, float *color, float widthLine);
void displayCube(windowContext *winParam, Cube_T *c);

#endif
