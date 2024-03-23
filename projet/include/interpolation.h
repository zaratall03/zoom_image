#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"

typedef enum {
    BILINEAR,
    HERMITE,
    NEAREST_NEIGHBOR
} ZoomType;


Image zoomOutBilinear(Image image, float zoomFactor) ;
Image zoomBilinear(Image image, float zoomFactor) ;

Image zoomNearestNeighbor(Image image, float zoomFactor) ;
Image zoomOutNearestNeighbor(Image image, float zoomFactor) ;

Image zoomOutHermite(Image image, float zoomFactor) ;
Image zoomHermite(Image image, float zoomFactor) ;



float hermiteInterpolation(float p0, float m0, float p1, float m1, float t) ;
float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v) ;



#endif