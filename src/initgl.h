
#include <stdint.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
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
    Window win;

    EGLNativeDisplayType eglNativeDisplay;
    EGLNativeWindowType eglNativeWindow;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;

    GLuint programObject;

    void (*keyFunc) (windowContext *winParam, char text);
};

GLuint INITGL_LoadTexture(const char *imagePath);
GLboolean INITGL_InitGL(windowContext *winParams);
int INITGL_InitEGL(windowContext *winParams, const char *title, GLint width, GLint height,
		GLuint flags);
