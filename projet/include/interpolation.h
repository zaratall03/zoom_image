#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"
#include <time.h>
#include <pthread.h>
#define NB_TYPE 3


typedef enum {
    NEAREST_NEIGHBOR = 0, 
    BILINEAR = 1,
    BICUBIQUE = 2, 
    // BICUBIQUE = 3, 
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

typedef Image (*Zoom) (Image, float ZoomFactor);

struct timespec getStartFromResult(ZoomType);
struct timespec getEndFromResult(ZoomType);
    
Image zoomNearestNeighbor(Image image, float zoomFactor) ;
Image zoomOutNearestNeighbor(Image image, float zoomFactor) ;

float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v);
Image zoomBilinear(Image image, float zoomFactor) ;
Image zoomOutBilinear(Image image, float zoomFactor) ;

double cubicInterpolate(double p[4], double x);
double bicubicInterpolate(Image *image, double x, double y, int channelIndex);
Image zoomBicubic(Image image, float zoomFactor); 
Image zoomOutBicubic(Image image, float zoomFactor);


struct timespec getStartFromResult(ZoomType type);
struct timespec getEndFromResult(ZoomType type);
Image getImageFromResult(ZoomType type);
void setStartFromResult(ZoomType type, struct timespec res);
void setEndFromResult(ZoomType type, struct timespec res);
void setImageFromResult(ZoomType type, Image res);

ZoomType getdisplayedZoomType();
void setdisplayedZoomType(ZoomType);

double calculateElapsedTime(struct timespec start, struct timespec end);
void afficheResult(Result  res);
void afficheResultTab(ResultTab resultTab);


#endif
