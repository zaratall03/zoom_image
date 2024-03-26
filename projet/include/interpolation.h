#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"
#include <time.h>
#include <pthread.h>
#define NB_TYPE 3

typedef enum {
    BILINEAR = 0,
    HERMITE = 1,
    NEAREST_NEIGHBOR = 2, 
} ZoomType;

typedef Image (*Zoom) (Image, float ZoomFactor);


typedef struct Result {
    struct timespec start, end; 
    Image resultImage;
    ZoomType zoomType;
} Result;

typedef struct ResultTab {
    int nbAlgo;
    Result results[NB_TYPE];
} ResultTab;


struct timespec getStartFromResult(ZoomType);
struct timespec getEndFromResult(ZoomType);
Image getImageFromResult(ZoomType);
    

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


struct timespec getStartFromResult(ZoomType type);
struct timespec getEndFromResult(ZoomType type);
Image getImageFromResult(ZoomType type);
void setStartFromResult(ZoomType type, struct timespec res);
void setEndFromResult(ZoomType type, struct timespec res);
void setImageFromResult(ZoomType type, Image res);



#endif
