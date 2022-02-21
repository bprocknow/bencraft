#include "cube.h"
#include "initgl.h"
#include "display.h"
#include "world.h"
#include "log.h"
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>

static GLboolean userInterrupt(windowContext *winParam)
{
    XEvent xev;
    KeySym key;
    GLboolean userinterrupt = GL_FALSE;
    char text;

    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while (XPending (winParam->xDisplay))
    {
        XNextEvent(winParam->xDisplay, &xev);
/*
	// TODO Implement key press
        if ( xev.type == KeyPress )
        {
            if (XLookupString(&xev.xkey,&text,1,&key,0)==1)
            {
                if (esContext->keyFunc != NULL)
                    esContext->keyFunc(esContext, text, 0, 0);
            }
        }
        if (xev.type == ClientMessage) {
            if (xev.xclient.data.l[0] == s_wmDeleteMessage) {
                userinterrupt = GL_TRUE;
            }
        }
*/
	if (xev.type == DestroyNotify) {
            userinterrupt = GL_TRUE;
	}
    }
    return userinterrupt;
}

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

    while(userInterrupt(winParam) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        
	setWorldOrient(winParam);

        handleErrors();

	eglSwapBuffers(winParam->eglDisplay, winParam->eglSurface);
    }
}

int main() {    
    // TODO Make method that gets uniform/attribute locations for a program

    const char *title = "BenCraft";

    windowContext winParam;

    if (!initEGL(&winParam, title, 800, 600, WINDOW_RGB)) {
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
