#ifndef WINDOW_H
#define WINDOW_H

#include "initgl.h"

void WIN_UpdateWindowSize(windowContext *winParam);
void WIN_GetMouseInput(windowContext *winParam, UserPosition_T *usrPos);
int WIN_UserInterrupt(windowContext *winParam, UserPosition_T *userPos);
int WIN_CreateWindow(windowContext *winParam, const char *title);

#endif
