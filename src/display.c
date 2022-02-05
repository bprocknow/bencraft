#include "display.h"
#include <math.h>

#include <sys/time.h>
#include "debug.h"

#define PI 3.14159265
#define DEGTORAD(x) (x * PI / 180.0)
// TODO make attribute structure containing attribute positions for different shaders



static const uint32_t FACEARR[12][3] = {
        {0, 1, 2},
	{1, 3, 2},
	{0, 4, 1},
	{1, 4, 5},
	{1, 5, 3},
	{3, 5, 7},
	{2, 3, 6},
	{3, 7, 6},
	{4, 7, 5},
	{4, 6, 7},
	{0, 6, 4},
	{0, 2, 6},
};

// TODO Remove - testing 
static const float COLORARR[3][4] = {
        {1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f}
};

/*
    Inputs:
    x, y, z rotations in degrees for model to world
    x, y, z translations for model to world

    Output: 
    4x4 rotation matrix
*/
void setOrient(float rotX, float rotY, float rotZ, float posX, float posY, float posZ, float *arr) {
    float radX, radY, radZ;
    radX = DEGTORAD(rotX);
    radY = DEGTORAD(rotY);
    radZ = DEGTORAD(rotZ);

    arr[0] = cosf(radZ)*cosf(radY);
    arr[1] = sinf(radX)*sinf(radY)*cosf(radZ) - cosf(radX)*sinf(radZ);
    arr[2] = cosf(radX)*sinf(radY)*cosf(radZ) + sinf(radX)*sinf(radZ);
    arr[3] = posX;
    arr[4] = cosf(radY)*sinf(radZ);
    arr[5] = sinf(radX)*sinf(radY)*sinf(radZ) + cosf(radX)*cosf(radZ);
    arr[6] = cosf(radX)*sinf(radY)*sinf(radZ) - sinf(radX)*cosf(radZ);
    arr[7] = posY;
    arr[8] = -sinf(radY);
    arr[9] = sinf(radX)*cosf(radY);
    arr[10] = cosf(radX)*cosf(radY);
    arr[11] = posZ;
    arr[12] = 0.0f;
    arr[13] = 0.0f;
    arr[14] = 0.0f;
    arr[15] = 1.0f;
}

void displayCube(windowContext *winParam, cube *c) {
    GLint vertexPos;
    GLint colorPos;
    GLint matPos;

    float arr[16];
    struct timeval te;
    gettimeofday(&te, NULL);
    long long millisec = (te.tv_sec*1000LL + te.tv_usec/1000)/10;
    setOrient(millisec%360, millisec%360, millisec%360, 0, 0, 0, arr);
    printMatrix4x4(arr);
    matPos = glGetUniformLocation(winParam->programObject, "matrix");
    glUniformMatrix4fv(matPos, 1, GL_FALSE, arr); 

    vertexPos = glGetAttribLocation(winParam->programObject, "vPosition");
    colorPos = glGetAttribLocation(winParam->programObject, "color");

    glEnableVertexAttribArray(vertexPos);
    glEnableVertexAttribArray(colorPos);

    // TODO Always enable?
    glEnable(GL_CULL_FACE);

    // Make two triangles out of each face of the cube.  CCW is pointing forward.
    for (int i = 0; i < 12; i++) {
        // Create matrix of each corner (x,y,z) that makes up half the face of one side of cube
	GLfloat verts[] = {
    	    c->pos[FACEARR[i][0]][0], c->pos[FACEARR[i][0]][1], c->pos[FACEARR[i][0]][2], 1.0f,
            c->pos[FACEARR[i][1]][0], c->pos[FACEARR[i][1]][1], c->pos[FACEARR[i][1]][2], 1.0f,
	    c->pos[FACEARR[i][2]][0], c->pos[FACEARR[i][2]][1], c->pos[FACEARR[i][2]][2], 1.0f
	};
	
	glVertexAttribPointer(colorPos, 4, GL_FLOAT, GL_FALSE, 0, COLORARR[0]); 
        glVertexAttribPointer(vertexPos, 4, GL_FLOAT, GL_FALSE, 0, verts);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glDisableVertexAttribArray(vertexPos);
    glDisableVertexAttribArray(colorPos);
    glDisable(GL_CULL_FACE);
} 
