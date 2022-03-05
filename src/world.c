#include "matrix.h"
#include "initgl.h"
#include "world.h"
#include "cube.h"
#include "octree.h"
#include "log.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define PI 3.14159265
#define DEGTORAD(x) (x * PI / 180.0)

#define mapEq(r, x, z) (r*sinf(x/(4*r)) + r*cosf(x/r) - r*sinf(x/r) - r*sinf(z/(4*r)) - r*cosf(z/r) + r*sinf(z/(2*r)))

// TODO Make aspect and FOV ratios user adjustable to min/max range of values
#define FOV 90.0 * PI / 180.0
#define NEAR 0.125
#define FAR 100.0

int gMapSize = 0;

// Find the minimum point in the world.  
// There probably in O(1) way to implement this with calc magic.  
static int worldFindMinimum(int r, int size) {
    int minimum = size;
    int maximum = 0;
    
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
	    //int y = (r*sinf(x/(4*r)) + r*cosf(x/r) - 2*r*sinf(x/r) - 2*r*sinf(z/(4*r)) - r*cosf(z/r) + r*sinf(z/(2*r)));
	    int y = (int)mapEq(r, x, z);

	    if (y < minimum) {
		minimum = y;
	    }
	    if (y > maximum) {
                maximum = y;
	    }
	}
    }
    printf("Maximum: %d\n", maximum);
    if (minimum < -size) {
        return -size;
    } else {
        return minimum;
    }
}

// f(x, z) = rand*sin(x/(4*r)) + rand*cos(x/rand) - 2*rand*sin(x/rand) - 2*rand*sin(y/(4*rand))
//		 - rand*cos(y/rand) + rand*sin(x/(2*rand))
static int worldAlg(int r, int mX, int mZ) {
    float x = (float)mX;
    float z = (float)mZ;
    
    //return (int)(r*sinf(x/(4*r)) + r*cosf(x/r) - 2*r*sinf(x/r) - 2*r*sinf(z/(4*r)) - r*cosf(z/r) + r*sinf(z/(2*r)));
    return (int)mapEq(r, x, z);
}

/*
    Using a seed value to generate a random number, use a function to generate the
    x, y, z values for the ground.
    
    @Param size: Size of world (size x size x size) MUST be 2**n

    **Note:  The world generated will be stored in an octree, which can become
    	     very large
*/
void WORLD_GenerateWorld(int size, int seedValue) {
    
    gMapSize = size; 

    srandom(seedValue);
    int rand = random();
    // The maximum/minimum height of the ground must be 0 < x < size - 10
    rand = rand % (size / 2 - 10);
    LOG("Random number: %d\n", rand);

    int worldMin = worldFindMinimum(rand, size);
    printf("World minimum: %d\n", worldMin);

    Cube_T *genCube;
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
	    int maxY = worldAlg(rand, x, z) - worldMin;
            if (maxY > size - 1) {
                maxY = size - 1;
	    }
//	    for (int y = 0; y <= maxY; y++) {
	    genCube = CUBE_GenerateCube(x, maxY, z, GROUND);
	    OCT_AddBlock(genCube);
//	    }
	}
    }
}

// TODO Input rotation/position vector
void WORLD_SetWorldOrient(windowContext *winParam, UserPosition_T *usrPos) {
    GLint matrixPos;
    float aspect = winParam->width / winParam->height;
    float matrix[16];
    float tmpMatrix[16];
    float rotateY = usrPos->mouseY * 360.0f / winParam->height;
    float rotateX = usrPos->mouseX * 360.0f / winParam->width;

    MAT_Identity(matrix);
    MAT_Translate(tmpMatrix, usrPos->posX, usrPos->posY, usrPos->posZ);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    //MAT_Rotate(tmpMatrix, 1, 0, 0, rotateY);
    //MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Rotate(tmpMatrix, 0, 1, 0, rotateX);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Rotate(tmpMatrix, 1, 0, 0, rotateY);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Perspective(tmpMatrix, aspect, FOV, NEAR, FAR);
    MAT_Multiply(tmpMatrix, matrix, matrix);

    matrixPos = glGetUniformLocation(winParam->programObject, "matrix");
    glUniformMatrix4fv(matrixPos, 1, GL_FALSE, matrix);
}


