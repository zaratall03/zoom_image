#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"
#include <time.h>

#define NB_TYPE 3


typedef Image (*Zoom) (Image, float ZoomFactor);

typedef enum {
    BILINEAR = 0,
    HERMITE = 1,
    NEAREST_NEIGHBOR = 2, 
} ZoomType;


typedef struct Result {
    struct timespec start, end; 
    Image resultImage;
    ZoomType zoomType;
} Result;

typedef struct ResultTab {
    int nbAlgo;
    Result results[NB_TYPE];
} ResultTab;


extern ResultTab resultTab;

Image zoomOutBilinear(Image image, float zoomFactor) ;
Image zoomBilinear(Image image, float zoomFactor) ;

Image zoomNearestNeighbor(Image image, float zoomFactor) ;
Image zoomOutNearestNeighbor(Image image, float zoomFactor) ;

Image zoomOutHermite(Image image, float zoomFactor) ;
Image zoomHermite(Image image, float zoomFactor) ;



float hermiteInterpolation(float p00, float p01, float p10, float p11, float t);
float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v);


void afficheResult(Result  res);
void afficheResultTab(ResultTab resultTab);

double calculateElapsedTime(struct timespec start, struct timespec end);

#endif