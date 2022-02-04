#include "cube.h"
#include "log.h"

#include <stddef.h>
#include <stdio.h>

#define CUBESIZE 0.1f

/**
    Params:
    @x: Position x of the cube on map
    @y: Position y of the cube on map
*/
void generateCube(uint16_t x, uint16_t y, uint16_t z, cube *genCube) {
    
    genCube->position[0] = (pointPosition) {.x = ((float)x) - CUBESIZE, .y = ((float)y) - CUBESIZE, .z = ((float)z) - CUBESIZE};  
    
    genCube->position[1] = (pointPosition) {.x = ((float)x) + CUBESIZE, .y = ((float)y) - CUBESIZE, .z = ((float)z) - CUBESIZE};  
    
    genCube->position[2] = (pointPosition) {.x = ((float)x) - CUBESIZE, .y = ((float)y) - CUBESIZE, .z = ((float)z) + CUBESIZE};  
    
    genCube->position[3] = (pointPosition) {.x = ((float)x) + CUBESIZE, .y = ((float)y) - CUBESIZE, .z = ((float)z) + CUBESIZE};  
    
    genCube->position[4] = (pointPosition) {.x = ((float)x) - CUBESIZE, .y = ((float)y) + CUBESIZE, .z = ((float)z) - CUBESIZE};  
    
    genCube->position[5] = (pointPosition) {.x = ((float)x) + CUBESIZE, .y = ((float)y) + CUBESIZE, .z = ((float)z) - CUBESIZE};  
    
    genCube->position[6] = (pointPosition) {.x = ((float)x) - CUBESIZE, .y = ((float)y) + CUBESIZE, .z = ((float)z) + CUBESIZE};  
    
    genCube->position[7] = (pointPosition) {.x = ((float)x) + CUBESIZE, .y = ((float)y) + CUBESIZE, .z = ((float)z) + CUBESIZE};  
    
}
