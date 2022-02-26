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
    int windowX, windowY;

    while(userInterrupt(winParam) == GL_FALSE) {
        
	updateWindowSize(winParam);
	getMouseInput(winParam, &windowX, &windowY);
	float x = (float)windowX * 360.0f / (float)winParam->width;
	float y = (float)windowY * 360.0f / (float)winParam->height;
	printf("X: %f\tY: %f\n", x, y);
	setWorldOrient(winParam, x, y);
        
	OCT_DrawMap(winParam);

        handleErrors();

	eglSwapBuffers(winParam->eglDisplay, winParam->eglSurface);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

int main() {    
    // TODO Make method that gets uniform/attribute locations for a program

    const char *title = "BenCraft";

    windowContext winParam;

    if (!initEGL(&winParam, title, 800, 600, WINDOW_RGB | WINDOW_DEPTH)) {
        fprintf(stderr, "Could not initialize EGL\n");
	return 0;
    }
    printf("EGL Initialized\n");
    if (!initGL(&winParam)) {
        fprintf(stderr, "ERROR initializing OpenGL failed\n");
	return 0;
    }
    loadTextureFaces(); 
    
    OCT_LoadMap("/home/bprocknow/repo/bencraft/maps/map4x4.txt");

    WinLoop(&winParam);

    return 1;
}
