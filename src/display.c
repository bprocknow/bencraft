#include "cube.h"
#include "initgl.h"
#include "matrix.h"
#include "display.h"
#include "debug.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Number of 2D lines to draw to make the line appear 3D cylindrical 
#define NUM_2D_SECTION 100

void DIS_DrawLine(windowContext *winParam, float *origin, float *end, float *color, float widthLine) {
    // Calculate vector moving origin of line to (0,0,0)
    float vector[3] = {end[0] - origin[0], end[1] - origin[1], end[2] - origin[2]};

    // Normalize vector
    float dist = sqrtf(powf(vector[0], 2.0f) + powf(vector[1], 2.0f) + powf(vector[2], 2.0f));
    vector[0] = vector[0] / dist;
    vector[1] = vector[1] / dist;
    vector[2] = vector[2] / dist;

    float normal[3] = {0.0f};

    // Vector mostly in x direction - Find normal in x/y plane (There are infinite arbitary normals)
    if (vector[0] > vector[1] && vector[0] > vector[2]) {
        // Calculate normal to vector along x,y plane.  Dot product w/ z=0
        normal[0] = vector[1];
	normal[1] = -vector[0];

        // Normalize normal vector to length width of line
        float tempNormNor1 = (-normal[0]/normal[1] + sqrtf(powf((normal[0]/normal[1]), 2.0f) + 4.0f*(powf(widthLine/2.0f, 2.0f)))) / 2.0f;
        float tempNormNor0 = normal[0] * tempNormNor1 / normal[1];
        normal[0] = tempNormNor0;
        normal[1] = tempNormNor1;
    } else if (vector[1] > vector[2]) {
	// Calculate normal to vector along x,y plane.  Dot product w/ z=0
        normal[0] = vector[1];
	normal[1] = -vector[0];

        // Normalize normal vector to length width of line
        float tempNormNor0 = (-normal[1]/normal[0] + sqrtf(powf((normal[1]/normal[0]), 2.0f) + 4.0f*(powf(widthLine/2.0f, 2.0f)))) / 2.0f;
        float tempNormNor1 = normal[1] * tempNormNor0 / normal[0];
        normal[0] = tempNormNor0;
        normal[1] = tempNormNor1;
    }else {
	// Vector mostly in z direction - Find normal in z/x plane
        // Calculate normal to vector along x,y plane.  Dot product w/ z=0
        normal[0] = vector[2];
	normal[2] = -vector[0];

        // Normalize normal vector to length width of line
        float tempNormNor0 = (-normal[2]/normal[0] + sqrtf(powf((normal[2]/normal[0]), 2.0f) + 4.0f*(powf(widthLine/2.0f, 2.0f)))) / 2.0f;
        float tempNormNor2 = normal[2] * tempNormNor0 / normal[0];
        normal[0] = tempNormNor0;
        normal[2] = tempNormNor2;
    }
    

    // Calculate four corners of line to draw
    float corners[] = {origin[0]+normal[0], origin[1]+normal[1], origin[2]+normal[2], 1.0f,
    		       origin[0]-normal[0], origin[1]-normal[1], origin[2]-normal[2], 1.0f,
		       end[0]+normal[0],    end[1]+normal[1],    end[2]+normal[2],    1.0f,
		       end[0]-normal[0],    end[1]-normal[1],    end[2]-normal[2],    1.0f};
    const GLushort INDICES[] = {0, 1, 2, 1, 3, 2};
    
    GLint vertexPos = glGetAttribLocation(winParam->lineProgramObject, "vPosition");
    glEnableVertexAttribArray(vertexPos);
    
    GLint colorPos = glGetUniformLocation(winParam->lineProgramObject, "color");
    glUniform4fv(colorPos, 1, color);

    glDisable(GL_CULL_FACE);

    // Draw the line around the inputted axis n times
    for (int i = 0; i < NUM_2D_SECTION; i++) {
        float rotCorners[16];
	for (int j = 0; j < 4; j++) {
            MAT_RotateArbitraryAxis(vector, 360.0f/NUM_2D_SECTION*i, &(corners[j*4]), &(rotCorners[j*4]));
	}
	glVertexAttribPointer(vertexPos, 4, GL_FLOAT, GL_FALSE, 0, rotCorners);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, INDICES);
    }

    glDisableVertexAttribArray(vertexPos);
}

// TODO make attribute structure containing attribute positions for different shaders

void displayCube(windowContext *winParam, Cube_T *c) {

    const uint32_t FACEARR[6][4] = {
        {1, 0, 3, 2},
	{5, 1, 7, 3},
    	{4, 5, 6, 7},
    	{0, 4, 2, 6},
	{5, 4, 1, 0},
    	{3, 2, 7, 6},
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

    float arr[8][3];
    int curPos = 0;

    // Loop through y, z, x [-1, 1] for sign to calculate position of 8 corners
    for (int signZ = -1; signZ < 2; signZ += 2) {
        for (int signY = -1; signY < 2; signY +=2 ) {
            for (int signX = -1; signX < 2; signX += 2) {
		
		arr[curPos][0] = c->x + signX*CUBEWIDTH;
	        arr[curPos][1] = c->y + signY*CUBEWIDTH;
		arr[curPos][2] = c->z + signZ*CUBEWIDTH;

	 	// Move position to next corner (x,y,z)
		curPos += 1;
	    }
	}
    }


    vertexPos = glGetAttribLocation(winParam->texProgramObject, "vPosition");
    textureCoordPos = glGetAttribLocation(winParam->texProgramObject, "a_textCoord");
    samplerLoc = glGetUniformLocation(winParam->texProgramObject, "s_texture");
    
    // TODO Always enable?
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(textureCoordPos);
    glEnableVertexAttribArray(vertexPos);

    // Make two triangles out of each face of the cube.  CCW is pointing forward.
    for (int i = 0; i < 6; i++) {
        if (c->face[i]) {
            continue;
	}
	// Create matrix of each corner (x,y,z) that makes up half the face of one side of cube
	GLfloat verts[] = {
    	    arr[FACEARR[i][0]][0], arr[FACEARR[i][0]][1], arr[FACEARR[i][0]][2], 1.0f,
            arr[FACEARR[i][1]][0], arr[FACEARR[i][1]][1], arr[FACEARR[i][1]][2], 1.0f,
	    arr[FACEARR[i][2]][0], arr[FACEARR[i][2]][1], arr[FACEARR[i][2]][2], 1.0f,
	    arr[FACEARR[i][3]][0], arr[FACEARR[i][3]][1], arr[FACEARR[i][3]][2], 1.0f
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
