#include "initgl.h"
#include "cube.h"
#include "log.h"

#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

GLuint gFaceTextures[NUMTYPES][6] = { 0 };

static const char *baseDir = "/home/bprocknow/repo/bencraft/textures/";

/*
    For each type of block, load the textures for each face
*/
void loadTextureFaces(void) {
    char dir[128] = "";
    for (int i = 0; i < NUMTYPES; i++) {
        switch (i) {
            case GROUND:
	    	strcat(strcat(dir, baseDir), "grassside.png");
		LOG(dir);
	        GLuint grassSide = loadTexture(dir);
		dir[0] = '\0';
		strcat(strcat(dir, baseDir), "grasstop.png");
		GLuint grassTop = loadTexture(dir);
		dir[0] = '\0';
		strcat(strcat(dir, baseDir), "ground.png");
		GLuint ground = loadTexture(dir);

		gFaceTextures[GROUND][0] = grassSide; 
	        gFaceTextures[GROUND][1] = grassSide;
		gFaceTextures[GROUND][2] = grassSide;
		gFaceTextures[GROUND][3] = grassSide; 
		gFaceTextures[GROUND][4] = ground;
		gFaceTextures[GROUND][5] = grassTop;
                
		break;
	    default:
	        LOG("Invalid type");
		break;
	}
    }
}

/**
    Params:
    @x: Position x of the cube on map
    @y: Position y of the cube on map (up)
    @z: Position z of the cube on map
*/
Cube_T *generateCube(uint16_t x, uint16_t y, uint16_t z, CubeType type) {

    int curPos = 0;
    
    Cube_T *genCube = malloc(sizeof(Cube_T));
    if (genCube == NULL) {
        fprintf(stderr, "ERROR: Out of memory to allocate cube\n");
        return NULL;
    }
    // Loop through y, z, x [-1, 1] for sign to calculate position of 8 corners
    for (int signZ = -1; signZ < 2; signZ += 2) {
        for (int signY = -1; signY < 2; signY +=2 ) {
            for (int signX = -1; signX < 2; signX += 2) {
		
		genCube->pos[curPos][0] = x + signX*CUBEWIDTH;
	        genCube->pos[curPos][1] = y + signY*CUBEWIDTH;
		genCube->pos[curPos][2] = z + signZ*CUBEWIDTH;

	 	// Move position to next corner (x,y,z)
		curPos += 1;
	    }
	}
    }
    genCube->x = x;
    genCube->y = y;
    genCube->z = z;

    genCube->type = type;
    
    return genCube;
}
