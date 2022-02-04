#include "cube.h"
#include "log.h"

#include <stddef.h>
#include <stdio.h>

#define CUBEWIDTH 0.5f;

/**
    Params:
    @x: Position x of the cube on map
    @y: Position y of the cube on map
*/
void generateCube(uint16_t x, uint16_t y, uint16_t z, cube *genCube) {
    int curPos = 0;
    
    // Loop through y, z, x [-1, 1] for sign to calculate position of 8 corners
    for (int signY = -1; signY < 2; signY += 2) {
        for (int signZ = -1; signZ < 2; signZ +=2 ) {
            for (int signX = -1; signX < 2; signX += 2) {
		
		genCube->pos[curPos][0] = x + signX*CUBEWIDTH;
	        genCube->pos[curPos][1] = y + signY*CUBEWIDTH;
		genCube->pos[curPos][2] = z + signZ*CUBEWIDTH;

		curPos += 1;
	    }
	}
    }
}
