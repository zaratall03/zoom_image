#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"

typedef enum {
    BILINEAR,
    HERMITE,
    NEAREST_NEIGHBOR
} ZoomType;

/**
 * Algo de Zooms
*/
Image zoomHermite(Image image, float zoomFactor, int centerX, int centerY);
Image zoomNearestNeighbor(Image image, float zoomFactor, int centerX, int centerY);
Image zoomBilinear(Image image, float zoomFactor, int centerX, int centerY);



/**
 * Fonctions supplémentaires
*/
float interpolateHermite(float p0, float p1, float p2, float p3, float t);

#endif