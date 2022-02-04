#include "display.h"
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

static void WinLoop(windowContext *winParam) {
    cube genCube;

    generateCube(0, 0, 0, &genCube);

    while(userInterrupt(winParam) == GL_FALSE) {
        displayCube(winParam, &genCube);    
        
	eglSwapBuffers(winParam->eglDisplay, winParam->eglSurface);
    }
}

int main() {    
    // TODO Make method that gets uniform/attribute locations for a program

    const char *title = "BenCraft";

    windowContext winParam;

    initEGL(&winParam, title, 800, 600, WINDOW_RGB);
    printf("EGL Initialized\n");
    initGL(&winParam);

    WinLoop(&winParam);

    return 1;
}
