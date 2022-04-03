#include "matrix.h"
#include <math.h>

#define PI 3.14159265
#define DEGTORAD(x) (x * PI / 180.0)

void MAT_Identity(float *arr) {
    arr[0] = 1.0f;
    arr[1] = 0.0f;
    arr[2] = 0.0f;
    arr[3] = 0.0f;
    arr[4] = 0.0f;
    arr[5] = 1.0f;
    arr[6] = 0.0f;
    arr[7] = 0.0f;
    arr[8] = 0.0f;
    arr[9] = 0.0f;
    arr[10] = 1.0;
    arr[11] = 0.0f;
    arr[12] = 0.0f;
    arr[13] = 0.0f;
    arr[14] = 0.0f;
    arr[15] = 1.0f;
}

/*
    Inputs: 
    float *axis:  Axis of rotation
    float degrees:  Degrees to rotate around axis of rotation
    float *rotatedVector:  Vector to rotate around axis of rotation
*/
void MAT_RotateArbitraryAxis(float *axis, float degrees, float *rotVec, float *dest) {
    float radians = DEGTORAD(degrees);

    float rotationMatrix[] = {
	(1-cosf(radians))*powf(axis[0], 2.0f) + cosf(radians), 
	(1-cosf(radians))*axis[0]*axis[1] + sinf(radians)*axis[2],
	(1-cosf(radians))*axis[0]*axis[2] - sinf(radians)*axis[1], 
        0.0f,
	(1-cosf(radians))*axis[0]*axis[1] - sinf(radians)*axis[2],
	(1-cosf(radians))*powf(axis[1], 2.0f) + cosf(radians),
	(1-cosf(radians))*axis[1]*axis[2] + sinf(radians)*axis[0],
        0.0f,
	(1-cosf(radians))*axis[0]*axis[2] + sinf(radians)*axis[1],
	(1-cosf(radians))*axis[1]*axis[2] - sinf(radians)*axis[0],
	(1-cosf(radians))*powf(axis[2], 2.0f) + cosf(radians),
        0.0f,
	0.0f,
	0.0f,
	0.0f,
	1.0f};

    MAT_MatrixVectorMultiply(rotationMatrix, rotVec, dest);
}

/*
    Inputs:
    x, y, z rotations in degrees for model to world
    x, y, z translations for model to world

    Output: 
    4x4 rotation matrix
*/
void MAT_Rotate(float *arr, float x, float y, float z, float angle) {
    float rad = DEGTORAD(angle);
    float lSin = sinf(rad);
    float lCos = cosf(rad);
    
    arr[0] = lCos + x*x*(1 - lCos);
    arr[1] = x*y*(1 - lCos) + z*lSin;
    arr[2] = x*z*(1 - lCos) - y*lSin;
    arr[3] = 0.0f;
    arr[4] = x*y*(1 - lCos) - z*lSin;
    arr[5] = lCos + y*y*(1 - lCos);
    arr[6] = y*z*(1 - lCos) + x*lSin;
    arr[7] = 0.0f;
    arr[8] = x*z*(1 - lCos) + y*lSin;
    arr[9] = y*z*(1 - lCos) - x*lSin;
    arr[10] = lCos + z*z*(1 - lCos);
    arr[11] = 0.0f;
    arr[12] = 0.0f;
    arr[13] = 0.0f;
    arr[14] = 0.0f;
    arr[15] = 1.0f;
}


void MAT_Translate(float *arr, float x, float y, float z) {
    arr[0] = 1.0f;
    arr[1] = 0.0f;
    arr[2] = 0.0f;
    arr[3] = 0.0f;
    arr[4] = 0.0f;
    arr[5] = 1.0f;
    arr[6] = 0.0f;
    arr[7] = 0.0f;
    arr[8] = 0.0f;
    arr[9] = 0.0f;
    arr[10] = 1.0f;
    arr[11] = 0.0f;
    arr[12] = x;
    arr[13] = y;
    arr[14] = z;
    arr[15] = 1.0f;
}

void MAT_Perspective(float *arr, float aspect, float fov, float zNear, float zFar) {
    arr[0] = 1.0f / (aspect * tanf(fov / 2));
    arr[1] = 0.0f;
    arr[2] = 0.0f;
    arr[3] = 0.0f;
    arr[4] = 0.0f;
    arr[5] = 1.0f / tanf(fov / 2);
    arr[6] = 0.0f;
    arr[7] = 0.0f;
    arr[8] = 0.0f;
    arr[9] = 0.0f;
    arr[10] = (-zFar - zNear) / (zFar - zNear);
    arr[11] = -1.0f;
    arr[12] = 0.0f;
    arr[13] = 0.0f;
    arr[14] = (-2.0f * zFar * zNear) / (zFar - zNear);
    arr[15] = 0.0f;
}

void MAT_MatrixVectorMultiply(float *matrix, float *vector, float *dest) {
    float result[4];

    // Column major
    for (int colArr = 0; colArr < 4; colArr++) {
        float total = 0;
	for (int rowArr = 0; rowArr < 4; rowArr++) {
            total += matrix[colArr + 4*rowArr] * vector[rowArr];        
	}
	result[colArr] = total;
    }

    for (int i = 0; i < 4; i++) {
        dest[i] = result[i];
    }
}

void MAT_MatrixMatrixMultiply(float *arr1, float *arr2, float *dest) {
    float result[16];

    // Column Major arrays
    for (int rowArr = 0; rowArr < 4; rowArr++) {
        for (int colArr = 0; colArr < 4; colArr++) {
            float total = 0;
            
	    // Add arr1 column with arr2 row
	    for (int elem = 0; elem < 4; elem++) {
                total += arr1[colArr + elem*4] * arr2[rowArr*4 + elem];
	    }
	    result[rowArr*4 + colArr] = total;
	}
    }
    // Copy result to destination matrix
    for (int i = 0; i < 16; i++) {
        dest[i] = result[i];
    }
}

