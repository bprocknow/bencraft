#include <stdint.h>

typedef enum {
    GROUND, 
    SKY
} cubeType;

typedef struct {
    float x;
    float y;
    float z;
} pointPosition;

typedef struct {
    pointPosition position[8];

    cubeType type; 
} cube;

void generateCube(uint16_t x, uint16_t y, uint16_t z, cube *genCube);
