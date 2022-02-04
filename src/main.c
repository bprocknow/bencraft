#include "initgl.h"
#include <stdio.h>
#include <stdlib.h>
// TODO Remove?
#include "cube.h"



int main(int argc, char **args) {    
    // TODO Make method that gets uniform/attribute locations for a program
    GLint rotationLocation;

    const char *title = "BenCraft";

    windowContext winParam;

    initEGL(&winParam, title, 800, 600, WINDOW_RGB);
    printf("EGL Initialized\n");
    initGL(&winParam);

    rotationLocation = glGetUniformLocation(winParam->program, "rotation");

/*    cube *genCube = malloc(sizeof(cube));
    if (genCube == NULL) {
        fprintf(stderr, "Could not alloc cube\n");
    }
*/
    cube genCube;
    generateCube(0, 7, 100, &genCube);
    displayCube(winParam, &genCube);
/*    // Main loop
    while (1) {
        
    }
*/
    return 1;
}
