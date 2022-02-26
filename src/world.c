#include "initgl.h"
#include "world.h"
#include "log.h"
#include <math.h>
#include <sys/time.h>

#define PI 3.14159265
#define DEGTORAD(x) (x * PI / 180.0)

/*
    Inputs:
    x, y, z rotations in degrees for model to world
    x, y, z translations for model to world

    Output: 
    4x4 rotation matrix
*/
static void setOrient(float rotX, float rotY, float rotZ, float posX, float posY, float posZ, float *arr) {
    float radX, radY, radZ;
    radX = DEGTORAD(rotX);
    radY = DEGTORAD(rotY);
    radZ = DEGTORAD(rotZ);

    arr[0] = cosf(radZ)*cosf(radY);
    arr[4] = cosf(radY)*sinf(radZ);
    arr[8] = -sinf(radY);
    arr[12] = 0.0f;
    arr[1] = sinf(radX)*sinf(radY)*cosf(radZ) - cosf(radX)*sinf(radZ);
    arr[5] = sinf(radX)*sinf(radY)*sinf(radZ) + cosf(radX)*cosf(radZ);
    arr[9] = sinf(radX)*cosf(radY);
    arr[13] = 0.0f;
    arr[2] = cosf(radX)*sinf(radY)*cosf(radZ) + sinf(radX)*sinf(radZ);
    arr[6] = cosf(radX)*sinf(radY)*sinf(radZ) - sinf(radX)*cosf(radZ);
    arr[10] = cosf(radX)*cosf(radY);
    arr[14] = 0.0f;
    arr[3] = posX;
    arr[7] = posY;
    arr[11] = posZ;
    arr[15] = 1.0f;
}

// TODO Input rotation/position vector
void setWorldOrient(windowContext *winParam, float mouseX, float mouseY) {
    GLint matPos;

    float arr[16];
    
//    setOrient(millisec%360, millisec%360, millisec%360, 0, 0, 0, arr);
    // Movement mouse X is a Z rotation
    // Movement mouse Y is a X rotation
    setOrient(mouseY, 0, -mouseX, 0, 0, 0, arr);
    matPos = glGetUniformLocation(winParam->programObject, "matrix");
    glUniformMatrix4fv(matPos, 1, GL_FALSE, arr); 

}


