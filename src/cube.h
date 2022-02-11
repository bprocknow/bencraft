#include <GL/gl.h>
#include <stdint.h>

typedef enum {
    GROUND,
    SKY,
    WOOD,
    NUMTYPES
} cubeType;

typedef struct {
    float pos[8][3];
    
    cubeType type;    
} cube;

extern GLuint gFaceTextures[NUMTYPES][6];

void loadTextureFaces(void);
void generateCube(uint16_t x, uint16_t y, uint16_t z, cube *genCube, cubeType cType);
