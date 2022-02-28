

typedef struct {
    int mouseX;
    int mouseY;

    float posX;
    float posY;
    float posZ;
} UserPosition_T;

void WORLD_SetWorldOrient(windowContext *winParam, UserPosition_T *usrPos);
