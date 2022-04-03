#include "debug.h"
#include "display.h"
#include <stdio.h>

// X axis: Red
// Y axis: Green
// Z axis: Blue
void DEB_DrawAxis(windowContext *winParam) {
    float origin[] = {0.0f, 0.0f, 0.0f};
    float axis[] = {10.0f, 0.0f, 0.0f};
    float color[] = {1.0f, 0.0f, 0.0f, 1.0f};

    DIS_DrawLine(winParam, origin, axis, color, 1.0f);

    axis[0] = 0.0f;
    axis[1] = 10.0f;
    color[0] = 0.0f;
    color[1] = 1.0f;

    DIS_DrawLine(winParam, origin, axis, color, 1.0f);

    axis[1] = 0.0f;
    axis[2] = 10.0f;
    color[1] = 0.0f;
    color[2] = 1.0f;

    DIS_DrawLine(winParam, origin, axis, color, 1.0f);
}

void printMatrix4x4(float *arr) {
    printf("[%f,\t%f\t%f\t%f,\n" \
           "%f\t%f\t%f\t%f,\n" \
	   "%f\t%f\t%f\t%f,\n" \
	   "%f\t%f\t%f\t%f]\n",
	   arr[0], arr[1], arr[2], arr[3],
	   arr[4], arr[5], arr[6], arr[7],
	   arr[8], arr[9], arr[10], arr[11],
	   arr[12], arr[13], arr[14], arr[15]);
}
