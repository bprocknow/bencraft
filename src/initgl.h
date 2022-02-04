#include <stdint.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#define WINDOW_RGB	0
#define WINDOW_ALPHA	1
#define WINDOW_DEPTH	2
#define WINDOW_STENCIL	4
#define WINDOW_MULTISAMPLE 8

typedef struct tagWindowContext windowContext;

struct tagWindowContext {
    uint32_t width, height;
    
    Display *xDisplay;

    EGLNativeDisplayType eglNativeDisplay;
    EGLNativeWindowType eglNativeWindow;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;

    GLuint programObject;

    void (*keyFunc) (windowContext *winParam, char text);
};

GLboolean initGL(windowContext *winParams);
GLboolean initEGL(windowContext *winParams, const char *title, GLint width, GLint height,
		GLuint flags);
GLboolean userInterrupt(windowContext *winParams);

