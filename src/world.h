#ifndef WORLD_H
#define WORLD_H

typedef struct {
    int mouseX;
    int mouseY;

    float posX;
    float posY;
    float posZ;
} UserPosition_T;

extern int gMapSize;

void WORLD_GenerateWorld(int size, int seedValue);
void WORLD_SetUserPosition(windowContext *winParam, UserPosition_T *usrPos, float fwdMove, float sideMove);
void WORLD_SetWorldOrient(windowContext *winParam, UserPosition_T *usrPos, float *matrix);

#endif
