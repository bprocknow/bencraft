#include "matrix.h"
#include "initgl.h"
#include "world.h"
#include "cube.h"
#include "octree.h"
#include "log.h"
#include <math.h>
#include <sys/time.h>

#define PI 3.14159265
#define DEGTORAD(x) (x * PI / 180.0)

// TODO Make aspect and FOV ratios user adjustable to min/max range of values
#define ASPECT 16.0 / 9.0
#define FOV 90.0 * PI / 180.0
#define NEAR 0.125
#define FAR 100.0

// TODO Input rotation/position vector
void WORLD_SetWorldOrient(windowContext *winParam, UserPosition_T *usrPos) {
    GLint matrixPos;

    float matrix[16];
    float tmpMatrix[16];
    float rotateY = -usrPos->mouseY * 360.0f / winParam->height;
    float rotateX = usrPos->mouseX * 360.0f / winParam->width;

    MAT_Identity(matrix);
    MAT_Translate(tmpMatrix, usrPos->posX, usrPos->posY, usrPos->posZ);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Rotate(tmpMatrix, 1, 0, 0, rotateY);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Rotate(tmpMatrix, 0, 1, 0, rotateX);
    MAT_Multiply(tmpMatrix, matrix, matrix);
    MAT_Perspective(tmpMatrix, ASPECT, FOV, NEAR, FAR);
    MAT_Multiply(tmpMatrix, matrix, matrix);

    matrixPos = glGetUniformLocation(winParam->programObject, "matrix");
    glUniformMatrix4fv(matrixPos, 1, GL_FALSE, matrix);
}


