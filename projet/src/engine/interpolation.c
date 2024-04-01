#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include "interpolation.h"
#include "image.h"
#include <pthread.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


extern ResultTab resultTab;
extern pthread_mutex_t lock;
extern ZoomType displayedZoomType; 



unsigned char interpolateHermite(const Image *image, float x, float y, int channel) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float tx = x - x0;
    float ty = y - y0;

    int index00 = (y0 * image->width + x0) * image->channels + channel;
    int index01 = (y0 * image->width + x1) * image->channels + channel;
    int index10 = (y1 * image->width + x0) * image->channels + channel;
    int index11 = (y1 * image->width + x1) * image->channels + channel;

    unsigned char v00 = image->data[index00];
    unsigned char v01 = image->data[index01];
    unsigned char v10 = image->data[index10];
    unsigned char v11 = image->data[index11];

    float interpolatedValue = (1 - tx) * (1 - ty) * v00 +
                              tx * (1 - ty) * v01 +
                              (1 - tx) * ty * v10 +
                              tx * ty * v11;

    if (interpolatedValue < 0) {
        return 0;
    } else if (interpolatedValue > 255) {
        return 255;
    } else {
        return (unsigned char)interpolatedValue;
    }
}

Image zoomHermite(const Image image, float zoomFactor) {
    printf("\nHermite");
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image zoomedImage;
    zoomedImage.width = newWidth;
    zoomedImage.height = newHeight;
    zoomedImage.channels = image.channels;
    zoomedImage.data = (unsigned char*)malloc(newWidth * newHeight * image.channels * sizeof(unsigned char));

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            float origX = x / zoomFactor;
            float origY = y / zoomFactor;
            for (int channel = 0; channel < image.channels; channel++) {
                zoomedImage.data[(y * newWidth + x) * image.channels + channel] = interpolateHermite(&image, origX, origY, channel);
            }
        }
    }

    return zoomedImage;
}



Image zoomOutNearestNeighbor(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomNearestNeighbor(image, zoomFactorInverse);
}
Image zoomNearestNeighbor(Image image, float zoomFactor) {
    printf("\nPPV");
    int newWidth = image.width * zoomFactor;
    int newHeight = image.height * zoomFactor;

    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = image.path;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = (int)(x / zoomFactor);
            int srcY = (int)(y / zoomFactor);
 
            for (int c = 0; c < image.channels; c++) {
                newImage.data[(y * newWidth + x) * image.channels + c] = 
                    image.data[(srcY * image.width + srcX) * image.channels + c];
            }
        }
    }
    return newImage;
}



float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v) {
    return (1 - u) * (1 - v) * p00 + u * (1 - v) * p10 + (1 - u) * v * p01 + u * v * p11;
}


Image zoomBilinear(Image image, float zoomFactor) {
    printf("\nBilinéaire"); 
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = NULL;

    if (newImage.data == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation de mémoire pour la nouvelle image.\n");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            float srcX = x / zoomFactor;
            float srcY = y / zoomFactor;

            int srcX0 = (int)floor(srcX);
            int srcX1 = (int)ceil(srcX);
            int srcY0 = (int)floor(srcY);
            int srcY1 = (int)ceil(srcY);

            srcX0 = CLAMP(srcX0, 0, image.width - 1);
            srcX1 = CLAMP(srcX1, 0, image.width - 1);
            srcY0 = CLAMP(srcY0, 0, image.height - 1);
            srcY1 = CLAMP(srcY1, 0, image.height - 1);

            float u = srcX - srcX0;
            float v = srcY - srcY0;

            for (int c = 0; c < image.channels; c++) {
                float p00 = image.data[(srcY0 * image.width + srcX0) * image.channels + c];
                float p01 = image.data[(srcY1 * image.width + srcX0) * image.channels + c];
                float p10 = image.data[(srcY0 * image.width + srcX1) * image.channels + c];
                float p11 = image.data[(srcY1 * image.width + srcX1) * image.channels + c];

                float interpolated = bilinearInterpolation(p00, p01, p10, p11, u, v);
                newImage.data[(y * newWidth + x) * image.channels + c] = (unsigned char)interpolated;
            }
        }
    }
    return newImage;
}



Image zoomOutBilinear(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomBilinear(image, zoomFactorInverse);
}



Image zoomOutHermite(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomHermite(image, zoomFactorInverse);
}




void afficheResult(Result res) {
    printf("ZoomType: %d\n", res.zoomType);
    printf("Start Time: %ld.%ld\n", res.start.tv_sec, res.start.tv_nsec);
    printf("End Time: %ld.%ld\n", res.end.tv_sec, res.end.tv_nsec);
    printf("Dimension image : %dx%d", res.resultImage.width, res.resultImage.height);
    calculateElapsedTime(res.start, res.end);
    printf("\n");
}

void afficheResultTab(ResultTab res) {
    printf("Nombre d'algorithmes: %d\n", res.nbAlgo);
    printf("Résultats:\n");
    for (int i = 0; i < res.nbAlgo; ++i) {
        printf("Résultat %d:\n", i);
        afficheResult(res.results[i]);
    }
}




double calculateElapsedTime(struct timespec start, struct timespec end) {
    double elapsedSeconds = (double)(end.tv_sec - start.tv_sec); 
    double elapsedNanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
    double res = elapsedSeconds + elapsedNanoseconds;
    return res;
}

struct timespec getStartFromResult(ZoomType type) {
    struct timespec start;
    pthread_mutex_lock(&lock);
    start = resultTab.results[type].start;
    pthread_mutex_unlock(&lock);
    return start;
}

struct timespec getEndFromResult(ZoomType type) {
    struct timespec end;
    pthread_mutex_lock(&lock);
    end = resultTab.results[type].end;
    pthread_mutex_unlock(&lock);
    return end;
}

Image getImageFromResult(ZoomType type) {
    Image img;
    pthread_mutex_lock(&lock);
    img = resultTab.results[type].resultImage;
    pthread_mutex_unlock(&lock);
    return img; 
}

void setStartFromResult(ZoomType type, struct timespec res) {
    pthread_mutex_lock(&lock);
    resultTab.results[type].start = res;
    pthread_mutex_unlock(&lock);
}

void setEndFromResult(ZoomType type, struct timespec res) {
    pthread_mutex_lock(&lock);
    resultTab.results[type].end = res;
    pthread_mutex_unlock(&lock);
}

void setImageFromResult(ZoomType type, Image res) {
    pthread_mutex_lock(&lock);
    resultTab.results[type].resultImage = res;
    pthread_mutex_unlock(&lock);
}



Image zoomOutBicubic(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomBicubic(image, zoomFactorInverse);
}


unsigned char cubicInterpolate(unsigned char p[4], unsigned char x) {
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}


// Image zoomBicubic(Image image, float zoomFactor) {
//     int newWidth = (int)(image.width * zoomFactor);
//     int newHeight = (int)(image.height * zoomFactor);

//     Image resizedImage;
//     resizedImage.width = newWidth;
//     resizedImage.height = newHeight;
//     resizedImage.channels = image.channels;
//     resizedImage.data = (unsigned char *)malloc(newWidth * newHeight * resizedImage.channels * sizeof(unsigned char));

//     float x_ratio = (float)(image.width - 1) / newWidth;
//     float y_ratio = (float)(image.height - 1) / newHeight;

//     for (int y = 0; y < newHeight; ++y) {
//         for (int x = 0; x < newWidth; ++x) {
//             float x_l = floor(x * x_ratio);
//             float y_l = floor(y * y_ratio);
//             float x_h = x_l + 1;
//             float y_h = y_l + 1;

//             float x_frac = x * x_ratio - x_l;
//             float y_frac = y * y_ratio - y_l;

//             unsigned char p[4];
//             for (int c = 0; c < image.channels; ++c) {
//                 p[0] = getPixelComposante(image, (int)x_l, (int)y_l, c);
//                 p[1] = getPixelComposante(image, (int)x_h, (int)y_l, c);
//                 p[2] = getPixelComposante(image, (int)x_h, (int)y_h, c);
//                 p[3] = getPixelComposante(image, (int)x_l, (int)y_h, c);

//                 resizedImage.data[(y * newWidth + x) * resizedImage.channels + c] = cubicInterpolate(p, x_frac);
//             }
//         }
//     }

//     return resizedImage;
// }

Image zoomBicubic(Image image, float zoomFactor) {
    printf("\nBicubique");
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image resizedImage;
    resizedImage.width = newWidth;
    resizedImage.height = newHeight;
    resizedImage.channels = image.channels;
    resizedImage.data = (unsigned char *)malloc(newWidth * newHeight * resizedImage.channels * sizeof(unsigned char));

    float x_ratio = (float)(image.width - 1) / newWidth;
    float y_ratio = (float)(image.height - 1) / newHeight;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float x_l = floor(x * x_ratio);
            float y_l = floor(y * y_ratio);
            float x_h = x_l + 1;
            float y_h = y_l + 1;

            float x_frac = x * x_ratio - x_l;
            float y_frac = y * y_ratio - y_l;

            unsigned char p[4];
            for (int c = 0; c < image.channels; ++c) {
                p[0] = getPixelComposante(image, (int)fmax(0, x_l), (int)fmax(0, y_l), c);
                p[1] = getPixelComposante(image, (int)fmin(image.width - 1, x_h), (int)fmax(0, y_l), c);
                p[2] = getPixelComposante(image, (int)fmin(image.width - 1, x_h), (int)fmin(image.height - 1, y_h), c);
                p[3] = getPixelComposante(image, (int)fmax(0, x_l), (int)fmin(image.height - 1, y_h), c);

                resizedImage.data[(y * newWidth + x) * resizedImage.channels + c] = cubicInterpolate(p, x_frac);
            }
        }
    }

    return resizedImage;
}



ZoomType getdisplayedZoomType(){
    pthread_mutex_lock(&lock);
    ZoomType res = displayedZoomType;
    pthread_mutex_unlock(&lock);
    return res;
}


void setdisplayedZoomType(ZoomType newType){
    pthread_mutex_lock(&lock);
    displayedZoomType =  newType;
    pthread_mutex_unlock(&lock);
}