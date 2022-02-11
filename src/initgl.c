#include "initgl.h"
#include "log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <math.h>
#include <png.h>
#include <errno.h>

static Atom s_wmDeleteMessage;
static const char *VERTEX_FILE = "/home/bprocknow/repo/bencraft/shaders/vertex.glsl";
static const char *FRAG_FILE = "/home/bprocknow/repo/bencraft/shaders/fragment.glsl";

static int readPNG(const char *imagePath, int *outWidth, int *outHeight, Bool *outHasAlpha, GLubyte **outData);
static EGLBoolean createWindow(windowContext *winParam, const char *title);
static char *readFile(const char *filePath);

/*
Read in a PNG file and format to use image as OpenGL texture

Resource used: 
https://blog.nobel-joergensen.com/2010/11/07/loading-a-png-as-texture-in-opengl-using-libpng/
*/
static int readPNG(const char *imagePath, int *outWidth, int *outHeight, Bool *outHasAlpha, GLubyte **outData) {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    
    fp = fopen(imagePath, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open image file %s\n", imagePath);
        return 0;
    }
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Could not create PNG read structure\n");
        fclose(fp);
        return 0;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr) {
        fprintf(stderr, "Could not create PNG info structure\n");
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }
    
    png_init_io(png_ptr, fp);
    
    /*
    * If you have enough memory to read
    * in the entire image at once, and
    * you need to specify only
    * transforms that can be controlled
    * with one of the PNG_TRANSFORM_*
    * bits (this presently excludes
    * dithering, filling, setting
    * background, and doing gamma
    * adjustment), then you can read the
    * entire image (including pixels)
    * into the info structure with this
    * call
    *
    * PNG_TRANSFORM_STRIP_16 |
    * PNG_TRANSFORM_PACKING  forces 8 bit
    * PNG_TRANSFORM_EXPAND forces to
    *  expand a palette into RGB
    */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
    
    png_uint_32 width, height;
    int bit_depth;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
    	 NULL, NULL, NULL);
    *outWidth = width;
    *outHeight = height;
    *outHasAlpha = (color_type == PNG_COLOR_TYPE_RGBA); 
    
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*) malloc(row_bytes * height);
    
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (int i = 0; i < (int)height; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);
    }
 
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 
    fclose(fp);
 
    return 1;
}

/** 
    Read glsl file for loading shaders
*/
GLuint loadTexture(const char *imagePath) {
    GLuint textureId;
    
    // TODO Read the header of the file and find what type of image it is.
    // TODO Be able to read in more than just PNG?
    int texWidth;
    int texHeight;
    Bool hasAlpha;
    GLubyte *textureImg;

    if (!readPNG(imagePath, &texWidth, &texHeight, &hasAlpha, &textureImg)) {
        fprintf(stderr, "ERROR: Could not read PNG file\n");
	return 0;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, 
    	texWidth, texHeight, 0, hasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureImg); 
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    LOG("Loaded texture: %s\tvalue: %d", imagePath, textureId);

    return textureId;
}

static char *readFile(const char *filePath) {
    FILE *fp;
    int curIdx = 0;
    int fileSize;
    const int bufSize = 512;
    char buf[bufSize];
    char *retBuf;
    
    fp = fopen(filePath, "rb");
    if (fp == NULL) {
        printf("Could not open file: %s, %s\n", filePath, strerror(errno));
	return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    retBuf = malloc(fileSize * sizeof(char) + 1);
    retBuf[fileSize *sizeof(char)] = '\0';

    while (fgets(buf, bufSize, fp)) {
	memcpy(retBuf+curIdx, buf, ftell(fp)-curIdx);
	curIdx = ftell(fp);
    }

    fclose(fp);

    return retBuf;
}

static GLuint loadShader(GLenum type, const char *shaderTxt) {
    GLuint shader;
    GLint compiled;

    // Create shader object
    shader = glCreateShader(type);
    if (!shader) {
        fprintf(stderr, "Could not create shader object\n");
	return 0;
    }

    // Set the source 
    glShaderSource(shader, 1, &shaderTxt, NULL);

    // Compile the source
    glCompileShader(shader);

    // Check compilation status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint infoLen = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);
	    
	    glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
	    fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);

	    free(infoLog);
	}
        if (type == GL_VERTEX_SHADER) {
	    fprintf(stderr, "Could not compile vertex shader\n");
	} else if (type == GL_FRAGMENT_SHADER) {
            fprintf(stderr, "Could not compile fragment shader\n");
	}
	glReleaseShaderCompiler();
	glDeleteShader(shader);
	return 0;
    }

    if (type == GL_FRAGMENT_SHADER) {
        glReleaseShaderCompiler();
    }

    return shader;
}

static GLboolean createProgram(windowContext *winParam) {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;
    char *shaderTxt;
    char *fragTxt;

    shaderTxt = readFile(VERTEX_FILE);
    if (!shaderTxt) {
        fprintf(stderr, "ERROR: Could not open file %s\n", VERTEX_FILE);
	return GL_FALSE;
    }
    fragTxt = readFile(FRAG_FILE);
    if (!shaderTxt) {
        fprintf(stderr, "ERROR: Could not open file %s\n", FRAG_FILE);
	return GL_FALSE;
    }
    vertexShader = loadShader(GL_VERTEX_SHADER, shaderTxt);
    if (!vertexShader) {
        return GL_FALSE;
    }
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragTxt);
    if (!fragmentShader) {
        return GL_FALSE;
    }
    free(shaderTxt);
    free(fragTxt);
    if (!vertexShader || !fragmentShader) {
        fprintf(stderr, "Could not load GL shaders\n");
	return GL_FALSE;
    }

    winParam->programObject = glCreateProgram();
    if (winParam->programObject == 0) {
        fprintf(stderr, "Could not create shader program\n");
	return GL_FALSE;
    }

    glAttachShader(winParam->programObject, vertexShader);
    glAttachShader(winParam->programObject, fragmentShader);
    glLinkProgram(winParam->programObject);

    // Make sure link succeeded
    glGetProgramiv(winParam->programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "Could not verify shader program.  Verify shader code\n");
	glDeleteProgram(winParam->programObject);
	return GL_FALSE;
    }

    // Verify that runtime operation will succeed
    glValidateProgram(winParam->programObject);
    glGetProgramiv(winParam->programObject, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "Validating program failed\n");
	glDeleteProgram(winParam->programObject);
	return GL_FALSE;
    }

    return GL_TRUE;
}

/** 
    initGL

    Initializes OpenGL by loading the shaders, GL program
*/
GLboolean initGL(windowContext *winParam) {
    
    GLboolean ret;

    ret = createProgram(winParam);
    if (!ret) {
        fprintf(stderr, "Could not create OpenGL program\n"); 
	return GL_FALSE;
    }
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glViewport(0, 0, winParam->width, winParam->height);

    glUseProgram(winParam->programObject);
    
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    return GL_TRUE;
}

void registerKeyFunc(windowContext *winParam, void (*keyFunc) (windowContext *, char)) {
    winParam->keyFunc = keyFunc;
}

/**
    initEGL

    Initialized egl context, surface and display
*/
GLboolean initEGL(windowContext *winParam, const char *title, GLint width, GLint height,
	GLuint flags) {
    EGLConfig config;
    EGLint major, minor;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    
    winParam->width = width;
    winParam->height = height;
    
    if (!createWindow(winParam, title)) {
	fprintf(stderr, "Failed to create X window\n");
        return GL_FALSE;
    }

    winParam->eglDisplay = eglGetDisplay(winParam->eglNativeDisplay);
    if (winParam->eglDisplay == EGL_NO_DISPLAY) {
        return GL_FALSE;
    }

    if (!eglInitialize(winParam->eglDisplay, &major, &minor)) {
        return GL_FALSE;
    }
    printf("EGL version %d.%d\n", major, minor);

    {
        EGLint numConfigs = 0;
	EGLint attribList[] = {
	    EGL_RED_SIZE,	5,
	    EGL_GREEN_SIZE,	6,
	    EGL_BLUE_SIZE,	5,
	    EGL_ALPHA_SIZE,	(flags & WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
	    EGL_DEPTH_SIZE, 	(flags & WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
	    EGL_STENCIL_SIZE, 	(flags & WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
	    EGL_SAMPLE_BUFFERS, (flags & WINDOW_MULTISAMPLE) ? 1: 0,
	    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	    EGL_NONE
	};

	if (!eglChooseConfig(winParam->eglDisplay, attribList, &config, 1, &numConfigs)) {
            return GL_FALSE;
	}
	if (numConfigs < 1) {
            return GL_FALSE;
	}
    }

    winParam->eglSurface = eglCreateWindowSurface(winParam->eglDisplay, config, 
    						winParam->eglNativeWindow, NULL);
    if (winParam->eglSurface == EGL_NO_SURFACE) {
        return GL_FALSE;
    }

    winParam->eglContext = eglCreateContext(winParam->eglDisplay, config, EGL_NO_CONTEXT,
    					contextAttribs);
    if (winParam->eglContext == EGL_NO_CONTEXT) {
        return GL_FALSE;
    }

    if (!eglMakeCurrent(winParam->eglDisplay, winParam->eglSurface, 
    			winParam->eglSurface, winParam->eglContext)) {
        return GL_FALSE;
    }

    return GL_TRUE;
}

/**
    createWindow

    Used to initialized native X11 display
*/
static EGLBoolean createWindow(windowContext *winParam, const char *title) {
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes xAttr;
    Atom wm_state;
    XWMHints hints;
    XEvent xEv;
    Window win;

    winParam->xDisplay = XOpenDisplay(NULL);
    if (!winParam->xDisplay) {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(winParam->xDisplay);

    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    printf("Creating Widnw \n");
    win = XCreateWindow(
    		winParam->xDisplay, root,
		0, 0, winParam->width, winParam->height, 0,
		CopyFromParent, InputOutput,
		CopyFromParent, CWEventMask,
		&swa);
    s_wmDeleteMessage = XInternAtom(winParam->xDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(winParam->xDisplay, win, &s_wmDeleteMessage, 1);

    xAttr.override_redirect = False;
    XChangeWindowAttributes(winParam->xDisplay, win, CWOverrideRedirect, &xAttr);

    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(winParam->xDisplay, win, &hints);

    XMapWindow(winParam->xDisplay, win);
    XStoreName(winParam->xDisplay, win, title);

    wm_state = XInternAtom(winParam->xDisplay, "_NET_WM_STATE", False);

    memset(&xEv, 0, sizeof(xEv));
    xEv.type 			= ClientMessage;
    xEv.xclient.window 		= win;
    xEv.xclient.message_type 	= wm_state;
    xEv.xclient.format	 	= 32;
    xEv.xclient.data.l[0]  	= 1;
    xEv.xclient.data.l[1]	= False;
    XSendEvent(winParam->xDisplay,
		DefaultRootWindow(winParam->xDisplay),
		False,
		SubstructureNotifyMask,
		&xEv);
   
   winParam->eglNativeWindow = (EGLNativeWindowType) win;
   winParam->eglNativeDisplay = (EGLNativeDisplayType) winParam->xDisplay;

   return EGL_TRUE;
}
