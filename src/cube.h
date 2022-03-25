
#include <GL/gl.h>
#include <stdbool.h>
#include <stdint.h>

#define CUBEWIDTH 0.5f

typedef enum {
    GROUND,
    GRASS,
    SKY,
    WOOD,
    NUMTYPES
} CubeType;

typedef struct {
    int x;		// Cube center position
    int y;		
    int z;

    CubeType type;
    bool surface;       // Cube is visible boolean
    bool face[6];	// Face visible boolean
} Cube_T;

extern GLuint gFaceTextures[NUMTYPES][6];

void CUBE_LoadTextureFaces(void);
Cube_T *CUBE_GenerateCube(uint16_t x, uint16_t y, uint16_t z, CubeType cType);
