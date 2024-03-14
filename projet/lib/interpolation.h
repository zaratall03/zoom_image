#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"


Image bilinear(const Image *inputImage, float zoom, int x, int y, int window_width, int window_height); 
float hermite(float p0, float m0, float p1, float m1, float t);
Image hermiteZoom(const Image *inputImage, float zoom);
Image nnbr(const Image *inputImage, float zoom);
#endif