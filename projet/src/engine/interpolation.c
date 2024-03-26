#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include "interpolation.h"
#include "image.h"
#include <pthread.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


extern ResultTab resultTab;
extern pthread_mutex_t lock;


float hermiteInterpolation(float p00, float p01, float p10, float p11, float t) {
        float c0 = p00;
    float c1 = p01;
    float c2 = -3 * p00 + 3 * p01 - 2 * p10 - p11;
    float c3 = 2 * p00 - 2 * p01 + p10 + p11;

    return c0 + c1 * t + c2 * t * t + c3 * t * t * t;
}

Image zoomHermite(Image image, float zoomFactor) {
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image outputImage;
    outputImage.width = newWidth;
    outputImage.height = newHeight;
    outputImage.channels = image.channels;
    outputImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels * sizeof(unsigned char));
    outputImage.path = NULL; 
    if (outputImage.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    int i, j;
    float x, y;

    for (i = 0; i < newHeight; i++) {
        for (j = 0; j < newWidth; j++) {
            x = (float)j / zoomFactor;
            y = (float)i / zoomFactor;

            int near_i = (int)y;
            int near_j = (int)x;

            near_i = near_i < 0 ? 0 : (near_i >= image.height - 1 ? image.height - 2 : near_i);
            near_j = near_j < 0 ? 0 : (near_j >= image.width - 1 ? image.width - 2 : near_j);

            float dx = x - near_j;
            float dy = y - near_i;

            for (int c = 0; c < image.channels; c++) {
                float p00 = image.data[(near_i * image.width + near_j) * image.channels + c];
                float p01 = image.data[(near_i * image.width + near_j + 1) * image.channels + c];
                float p10 = image.data[((near_i + 1) * image.width + near_j) * image.channels + c];
                float p11 = image.data[((near_i + 1) * image.width + near_j + 1) * image.channels + c];

                float interpolated = hermiteInterpolation(p00, p01, p10, p11, dx);

                outputImage.data[(i * newWidth + j) * image.channels + c] = (unsigned char)interpolated;
            }
        }
    
    }
    return outputImage;
}



Image zoomOutNearestNeighbor(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomNearestNeighbor(image, zoomFactorInverse);
}
Image zoomNearestNeighbor(Image image, float zoomFactor) {
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
    double elapsedNanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0; // Conversion en secondes
    double res = elapsedSeconds + elapsedNanoseconds;
    printf("\n Temps mis par l'algo : %f secondes", res );
    return res;
}

struct timespec getStartFromResult(ZoomType type) {
    struct timespec start;
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            start = resultTab.results[BILINEAR].start;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            start = resultTab.results[HERMITE].start;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            start = resultTab.results[NEAREST_NEIGHBOR].start;
            pthread_mutex_unlock(&lock);
            break;
    }
    return start;
}

struct timespec getEndFromResult(ZoomType type) {
    struct timespec end;
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            end = resultTab.results[BILINEAR].end;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            end = resultTab.results[HERMITE].end;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            end = resultTab.results[NEAREST_NEIGHBOR].end;
            pthread_mutex_unlock(&lock);
            break;
    }
    return end;
}

Image getImageFromResult(ZoomType type) {
    Image img;
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            img = resultTab.results[BILINEAR].resultImage;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            img = resultTab.results[HERMITE].resultImage;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            img = resultTab.results[NEAREST_NEIGHBOR].resultImage;
            pthread_mutex_unlock(&lock);
            break;
    }
    return img; 
}

void setStartFromResult(ZoomType type, struct timespec res) {
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            resultTab.results[BILINEAR].start = res;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            resultTab.results[HERMITE].start = res;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            resultTab.results[NEAREST_NEIGHBOR].start = res;
            pthread_mutex_unlock(&lock);
            break;
    }
    pthread_mutex_unlock(&lock);
}

void setEndFromResult(ZoomType type, struct timespec res) {
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            resultTab.results[BILINEAR].end = res;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            resultTab.results[HERMITE].end = res;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            resultTab.results[NEAREST_NEIGHBOR].end = res;
            pthread_mutex_unlock(&lock);
            break;
    }
}

void setImageFromResult(ZoomType type, Image res) {
    switch(type) {
        case BILINEAR:
            pthread_mutex_lock(&lock);
            resultTab.results[BILINEAR].resultImage = res;
            pthread_mutex_unlock(&lock);
            break;
        case HERMITE:
            pthread_mutex_lock(&lock);
            resultTab.results[HERMITE].resultImage = res;
            pthread_mutex_unlock(&lock);
            break;
        case NEAREST_NEIGHBOR:
            pthread_mutex_lock(&lock);
            resultTab.results[NEAREST_NEIGHBOR].resultImage = res;
            pthread_mutex_unlock(&lock);
            break;
    }
}
