

#include "initgl.h"
#include "world.h"
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define RIGHTCLICKMASK 1024
#define LEFTCLICKMASK 256

static Atom s_wmDeleteMessage;

void WIN_UpdateWindowSize(windowContext *winParam) {
    Window unusedWin;
    int unusedCornerX, unusedCornerY;
    unsigned int unusedBorderWidth, unusedDepth;

    XGetGeometry(winParam->xDisplay, winParam->win, &unusedWin, &unusedCornerX, &unusedCornerY, 
      &winParam->width, &winParam->height, &unusedBorderWidth, &unusedDepth);
}

void WIN_GetMouseInput(windowContext *winParam, UserPosition_T *usrPos) {
    Window unusedRootWin, unusedChildWin;
    int unusedX, unusedY;
    unsigned int mask;

    XQueryPointer(winParam->xDisplay, winParam->win, &unusedRootWin, &unusedChildWin, &unusedX, &unusedY, &usrPos->mouseX, &usrPos->mouseY, &mask);
    if (mask & RIGHTCLICKMASK) {
        ;
    }
    if (mask & LEFTCLICKMASK) {
        ;
    }
}

static void handleText(UserPosition_T *userPos, char text) {
    switch (text) {
        case 'w':
            userPos->posZ -= 0.1f;
	    break;
	case 'a':
	    userPos->posX -= 0.1f;
	    break;
	case 's':
	    userPos->posZ += 0.1f;
	    break;
	case 'd':
            userPos->posX += 0.1f;
	    break;
        case 'z':
	    userPos->posY += 0.1f;
	    break;
	case 'x': 
	    userPos->posY -= 0.1f;
	    break;
	default:
	    break;
    }
}

int WIN_UserInterrupt(windowContext *winParam, UserPosition_T *userPos) {
    XEvent xev;
    KeySym key;
    int userInterBool = 0;
    char text;

    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while (XPending (winParam->xDisplay))
    {
        XNextEvent(winParam->xDisplay, &xev);

	// TODO Implement key press
        if ( xev.type == KeyPress ) {
	    printf("Detected keypress\n");
            if (XLookupString(&xev.xkey, &text, 1, &key, 0) == 1) {
                printf("Keypress: %c\n", text);
                handleText(userPos, text);
	    }
        }
/*        if (xev.type == ClientMessage) {
            if (xev.xclient.data.l[0] == s_wmDeleteMessage) {
                userinterrupt = GL_TRUE;
            }
        }
*/
	if (xev.type == DestroyNotify) {
            printf("User interrupt\n");
            userInterBool = GL_TRUE;
	}
    }
    return userInterBool;
}


/**
    createWindow

    Used to initialized native X11 display
*/
int WIN_CreateWindow(windowContext *winParam, const char *title) {
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes xAttr;
    Atom wm_state;
    XWMHints hints;
    XEvent xEv;

    winParam->xDisplay = XOpenDisplay(NULL);
    if (!winParam->xDisplay) {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(winParam->xDisplay);

    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    printf("Creating Window \n");
    winParam->win = XCreateWindow(
                winParam->xDisplay, root,
                0, 0, winParam->width, winParam->height, 0,
                CopyFromParent, InputOutput,
                CopyFromParent, CWEventMask,
                &swa);
    s_wmDeleteMessage = XInternAtom(winParam->xDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(winParam->xDisplay, winParam->win, &s_wmDeleteMessage, 1);

    xAttr.override_redirect = False;
    XChangeWindowAttributes(winParam->xDisplay, winParam->win, CWOverrideRedirect, &xAttr);

    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(winParam->xDisplay, winParam->win, &hints);

    XMapWindow(winParam->xDisplay, winParam->win);
    XStoreName(winParam->xDisplay, winParam->win, title);

    wm_state = XInternAtom(winParam->xDisplay, "_NET_WM_STATE", False);

    memset(&xEv, 0, sizeof(xEv));
    xEv.type                    = ClientMessage;
    xEv.xclient.window          = winParam->win;
    xEv.xclient.message_type    = wm_state;
    xEv.xclient.format          = 32;
    xEv.xclient.data.l[0]       = 1;
    xEv.xclient.data.l[1]       = False;
    XSendEvent(winParam->xDisplay,
                DefaultRootWindow(winParam->xDisplay),
                False,
                SubstructureNotifyMask,
                &xEv);

   winParam->eglNativeWindow = (EGLNativeWindowType) winParam->win;
   winParam->eglNativeDisplay = (EGLNativeDisplayType) winParam->xDisplay;

   return 1;
}
