#include <GL/gl.h>
#include <stdint.h>

#define CUBEWIDTH 0.5f;

typedef enum {
    GROUND,
    SKY,
    WOOD,
    NUMTYPES
} CubeType;

typedef struct {
    float pos[8][3];	// Cube corner position
    int x;		// Cube center position
    int y;		
    int z;

    CubeType type;    
} Cube_T;

extern GLuint gFaceTextures[NUMTYPES][6];

void loadTextureFaces(void);
Cube_T *generateCube(uint16_t x, uint16_t y, uint16_t z, CubeType cType);
