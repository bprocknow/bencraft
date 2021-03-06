#include "debug.h"
#include "cube.h"
#include "initgl.h"
#include "display.h"
#include "world.h"
#include "log.h"
#include "window.h"
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>

static void handleErrors() {
    GLenum errorType;

    errorType = glGetError();
    switch (errorType) {
        case GL_NO_ERROR || GL_INVALID_ENUM:
	    break;
	case GL_INVALID_VALUE:
	    LOG("OpenGL numeric value out of range.");
	    break;
        case GL_INVALID_OPERATION:
	    LOG("OpenGL invalid operation occuring");
	    break;
	case GL_OUT_OF_MEMORY:
	    LOG("OpenGL insufficient memory to execute commands");
	    break;
    }
}

static void WinLoop(windowContext *winParam) {
    float matrix[16];
    GLuint matrixPos;
    UserPosition_T userPos;

    userPos.posX = 0.0f;
    userPos.posY = 0.0f;
    userPos.posZ = 0.0f;

    while(WIN_UserInterrupt(winParam, &userPos) == GL_FALSE) {
        // Update the width/height stored in windowContext when user incr/decr screen size
	WIN_UpdateWindowSize(winParam);
        glViewport(0, 0, winParam->width, winParam->height);

	WIN_GetMouseInput(winParam, &userPos);
	
	WORLD_SetWorldOrient(winParam, &userPos, matrix);
        
	// Draw everything that uses texture shader program
	glUseProgram(winParam->texProgramObject);

	matrixPos = glGetUniformLocation(winParam->texProgramObject, "matrix");
        glUniformMatrix4fv(matrixPos, 1, GL_FALSE, matrix);
        
	OCT_DrawMap(winParam);

        // Draw everything that uses line shader program
	glUseProgram(winParam->lineProgramObject);

	matrixPos = glGetUniformLocation(winParam->lineProgramObject, "matrix");
	glUniformMatrix4fv(matrixPos, 1, GL_FALSE, matrix);
        
        DEB_DrawAxis(winParam);

        // Handle OpenGL Errors
        handleErrors();

	eglSwapBuffers(winParam->eglDisplay, winParam->eglSurface);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

int main() {    
    // TODO Make method that gets uniform/attribute locations for a program

    const char *title = "BenCraft";

    windowContext winParam;

    if (!INITGL_InitEGL(&winParam, title, 800, 600, WINDOW_RGB | WINDOW_DEPTH)) {
        fprintf(stderr, "Could not initialize EGL\n");
	return 0;
    }
    printf("EGL Initialized\n");
    if (!INITGL_InitGL(&winParam)) {
        fprintf(stderr, "ERROR initializing OpenGL failed\n");
	return 0;
    }
    CUBE_LoadTextureFaces(); 
    
    WORLD_GenerateWorld(8, 1);
    
    //OCT_LoadMap("/home/bprocknow/repo/bencraft/maps/map8x8.txt");

    WinLoop(&winParam);

    return 1;
}
