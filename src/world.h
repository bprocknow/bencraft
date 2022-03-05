

typedef struct {
    int mouseX;
    int mouseY;

    float posX;
    float posY;
    float posZ;
} UserPosition_T;

extern int gMapSize;

void WORLD_GenerateWorld(int size, int seedValue);
void WORLD_SetWorldOrient(windowContext *winParam, UserPosition_T *usrPos);
