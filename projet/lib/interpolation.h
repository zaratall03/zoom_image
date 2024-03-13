#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H


Image bilinear(const Image *inputImage, float zoom); 
float hermite(float p0, float m0, float p1, float m1, float t);
Image hermiteZoom(const Image *inputImage, float zoom);
Image nnbr(const Image *inputImage, float zoom);
#endif