#include "cube.h"

// TODO make attribute structure containing attribute positions for different shaders
#define VERTEXPOS 0
#define ROTATIONPOS 1

static void displayFace(float coords[4]) {

}

void displayCube(windowContext *winParam, cube *c) {
    uint32_t faceArray[6][4] = {
    	{0, 1, 2, 3},
	{0, 1, 4, 5},
	{0, 2, 4, 6},
	{1, 3, 5, 7}, 
	{2, 3, 6, 7},
	{4, 5, 6, 7}
    };
    GLint rotation = 0;
    GLint rotPos;

    rotPos = glGetUniformLocation(winParam->program, "rotation");
    glUniform1f(rotPos, rotation);

    glEnableVertexAttribArray(VERTEXPOS);
    // TODO Always enable?
    glEnable(GL_CULL_FACE);

    // Loop through faces of cube
    for (int i = 0; i < 6; i++) {
        // Draw two triangles to make square
        for (int j = 0; j < 2; j++)
            GLfloat verts[] = {
			faceArray[i][j], 
			faceArray[i][j+1], 
			faceArray[i][j+2]
      			};
            glVertexAttribPointer(VERTEXPOS, 3, GL_FLOAT, GL_FALSE, verts);
    }

    glDisableVertexAttribArray(VERTEXPOS);
    glDisable(GL_CULL_FACE);
} 
