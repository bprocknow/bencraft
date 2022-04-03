#ifndef MATRIX_H
#define MATRIX_H

void MAT_Rotate(float *arr, float x, float y, float z, float angle);
void MAT_Identity(float *arr);
void MAT_Translate(float *arr, float x, float y, float z);
void MAT_RotateArbitraryAxis(float *axis, float degrees, float *rotVec, float *dest);
void MAT_MatrixVectorMultiply(float *matrix, float *vector, float *dest);
void MAT_MatrixMatrixMultiply(float *arr1, float *arr2, float *dest);
void MAT_Perspective(float *arr, float aspect, float fov, float zNear, float zFar);

#endif
