#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H
#include "image.h"
#include <time.h>
#include <pthread.h>
#define NB_TYPE 4


typedef enum {
    NEAREST_NEIGHBOR = 0, 
    BILINEAR = 1,
    HERMITE = 2, 
    BICUBIQUE = 3, 
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
    

Image zoomOutBilinear(Image image, float zoomFactor) ;
Image zoomBilinear(Image image, float zoomFactor) ;
float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v);

Image zoomNearestNeighbor(Image image, float zoomFactor) ;
Image zoomOutNearestNeighbor(Image image, float zoomFactor) ;

Image zoomOutHermite(Image image, float zoomFactor) ;
Image zoomHermite(Image image, float zoomFactor) ;
Image* zoomH(const Image *image, float zoomFactor);
unsigned char interpolateHermite(const Image *image, float x, float y, int channel);

void afficheResult(Result  res);
void afficheResultTab(ResultTab resultTab);

double calculateElapsedTime(struct timespec start, struct timespec end);

Image zoomBicubic(Image image, float zoomFactor);
Image zoomOutBicubic(Image image, float zoomFactor);
unsigned char cubicInterpolate(unsigned char p[4], unsigned char x); 


struct timespec getStartFromResult(ZoomType type);
struct timespec getEndFromResult(ZoomType type);
Image getImageFromResult(ZoomType type);
void setStartFromResult(ZoomType type, struct timespec res);
void setEndFromResult(ZoomType type, struct timespec res);
void setImageFromResult(ZoomType type, Image res);




#endif
