#include "cube.h"
#include "initgl.h"
#include "display.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"

// TODO make attribute structure containing attribute positions for different shaders

void displayCube(windowContext *winParam, cube *c) {

    static const uint32_t FACEARR[6][4] = {
        {0, 1, 2, 3},
	{1, 5, 3, 7},
    	{5, 4, 7, 6},
    	{4, 0, 6, 2},
	{4, 5, 0, 1},
    	{2, 3, 6, 7},
    };
    static const GLushort INDICES[] = {
        0, 1, 2, 1, 3, 2
    };
    static const GLfloat textInd[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    // TODO put in/create struct for ground program drawing
    GLint vertexPos;
    GLint textureCoordPos;
    GLint samplerLoc;

    vertexPos = glGetAttribLocation(winParam->programObject, "vPosition");
    textureCoordPos = glGetAttribLocation(winParam->programObject, "a_textCoord");
    samplerLoc = glGetUniformLocation(winParam->programObject, "s_texture");
    
    // TODO Always enable?
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(textureCoordPos);
    glEnableVertexAttribArray(vertexPos);

    // Make two triangles out of each face of the cube.  CCW is pointing forward.
    for (int i = 0; i < 6; i++) {
        // Create matrix of each corner (x,y,z) that makes up half the face of one side of cube
	GLfloat verts[] = {
    	    c->pos[FACEARR[i][0]][0], c->pos[FACEARR[i][0]][1], c->pos[FACEARR[i][0]][2], 1.0f,
            c->pos[FACEARR[i][1]][0], c->pos[FACEARR[i][1]][1], c->pos[FACEARR[i][1]][2], 1.0f,
	    c->pos[FACEARR[i][2]][0], c->pos[FACEARR[i][2]][1], c->pos[FACEARR[i][2]][2], 1.0f,
	    c->pos[FACEARR[i][3]][0], c->pos[FACEARR[i][3]][1], c->pos[FACEARR[i][3]][2], 1.0f
	};
	
        glVertexAttribPointer(textureCoordPos, 2, GL_FLOAT, GL_FALSE, 0, textInd);        
	glVertexAttribPointer(vertexPos, 4, GL_FLOAT, GL_FALSE, 0, verts);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gFaceTextures[c->type][i]);

        glUniform1i(samplerLoc, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, INDICES);
    }

    glDisableVertexAttribArray(vertexPos);
    glDisableVertexAttribArray(textureCoordPos);
    glDisable(GL_CULL_FACE);
} 
