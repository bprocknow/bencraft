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
void CUBE_LoadTextureFaces(void) {
    char dir[128] = "";

    strcat(strcat(dir, baseDir), "grassside.png");
    GLuint grassSide = INITGL_LoadTexture(dir);
    dir[0] = '\0';
    strcat(strcat(dir, baseDir), "grasstop.png");
    GLuint grassTop = INITGL_LoadTexture(dir);
    dir[0] = '\0';
    strcat(strcat(dir, baseDir), "ground.png");
    GLuint ground = INITGL_LoadTexture(dir);

    for (int i = 0; i < NUMTYPES; i++) {
        switch (i) {
            case GRASS:
		gFaceTextures[GRASS][0] = grassSide; 
	        gFaceTextures[GRASS][1] = grassSide;
		gFaceTextures[GRASS][2] = grassSide;
		gFaceTextures[GRASS][3] = grassSide; 
		gFaceTextures[GRASS][4] = ground;
		gFaceTextures[GRASS][5] = grassTop;
                break;
            case GROUND:
		gFaceTextures[GROUND][0] = ground; 
	        gFaceTextures[GROUND][1] = ground;
		gFaceTextures[GROUND][2] = ground;
		gFaceTextures[GROUND][3] = ground; 
		gFaceTextures[GROUND][4] = ground;
                gFaceTextures[GROUND][5] = ground;         
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
Cube_T *CUBE_GenerateCube(uint16_t x, uint16_t y, uint16_t z, CubeType type) {

    Cube_T *genCube = malloc(sizeof(Cube_T));
    if (genCube == NULL) {
        fprintf(stderr, "ERROR: Out of memory to allocate cube\n");
        return NULL;
    }
    
    genCube->x = x;
    genCube->y = y;
    genCube->z = z;

    genCube->type = type;

    genCube->surface = false;

    genCube->face[0] = false;
    genCube->face[1] = false;
    genCube->face[2] = false;
    genCube->face[3] = false;
    genCube->face[4] = false;
    genCube->face[5] = false;
    

    return genCube;
}
