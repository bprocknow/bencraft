#include "display.h"

// TODO make attribute structure containing attribute positions for different shaders
#define VERTEXPOS 0
#define ROTATIONPOS 1

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


void displayCube(windowContext *winParam, cube *c) {
    GLint rotation = 0;
    GLint rotPos;

    rotPos = glGetUniformLocation(winParam->programObject, "rotation");
    glUniform1f(rotPos, rotation);

    glEnableVertexAttribArray(VERTEXPOS);
    // TODO Always enable?
    glEnable(GL_CULL_FACE);

    // Make two triangles out of each face of the cube.  CCW is pointing forward.
    for (int i = 0; i < 12; i++) {
        // Create matrix of a x,y,z of each corner that makes up half the face of one side of cube
	GLfloat verts[] = {
    	    c->pos[FACEARR[i][0]][0], c->pos[FACEARR[i][0]][1], c->pos[FACEARR[i][0]][2],
            c->pos[FACEARR[i][1]][0], c->pos[FACEARR[i][1]][1], c->pos[FACEARR[i][1]][2],
	    c->pos[FACEARR[i][2]][0], c->pos[FACEARR[i][2]][1], c->pos[FACEARR[i][2]][2]
    	};
        glVertexAttribPointer(VERTEXPOS, 3, GL_FLOAT, GL_FALSE, 0, verts);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glDisableVertexAttribArray(VERTEXPOS);
    glDisable(GL_CULL_FACE);
} 
