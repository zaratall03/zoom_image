#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "image.h"
#include "interpolation.h"


Image bilinear(const Image *inputImage, float zoom) {
    Image outputImage;
    int r = inputImage->height;
    int c = inputImage->width;

    // Calcul de la taille de l'mage zoomée
    int intZoom = (int) floor(zoom);
    outputImage.width = c * intZoom;
    outputImage.height = r * intZoom;
    outputImage.channels = inputImage->channels;

    // Allocation de la mémoire pour l'image zoomée
    outputImage.data = (unsigned char *)malloc(outputImage.width * outputImage.height * outputImage.channels);
    
    if (outputImage.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy, fx, fy, value;

    for (i = 0; i < outputImage.height; i++) {
        for (j = 0; j < outputImage.width; j++) {
            x = j / zoom;
            y = i / zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;
            ii = near_i * c;
            jj = near_j;
            fx = 1 - dx;
            fy = 1 - dy;
            value = fx * (fy * inputImage->data[ii + jj] + dy * inputImage->data[ii + jj + c]) +
                    dx * (fy * inputImage->data[ii + jj + 1] + dy * inputImage->data[ii + jj + c + 1]);
            outputImage.data[i * outputImage.width + j] = (unsigned char)value;
        }
    }

    return outputImage;
}


float hermite(float p0, float m0, float p1, float m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float a = 2.0 * t3 - 3.0 * t2 + 1.0;
    float b = t3 - 2.0 * t2 + t;
    float c = -2.0 * t3 + 3.0 * t2;
    float d = t3 - t2;
    return a * p0 + b * m0 + c * p1 + d * m1;
}


Image hermiteZoom(const Image *inputImage, float zoom) {
    Image outputImage;
    int r = inputImage->height;
    int c = inputImage->width;
    int intZoom = (int)  floor(zoom);
    outputImage.width = c * intZoom;
    outputImage.height = r * intZoom;
    outputImage.channels = inputImage->channels;

    // Allocation de la mémoire pour l'image zoomée
    outputImage.data = (unsigned char *)malloc(outputImage.width * outputImage.height * outputImage.channels);
    
    if (outputImage.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy;

    for (i = 0; i < outputImage.height; i++) {
        for (j = 0; j < outputImage.width; j++) {
            x = j / zoom;
            y = i / zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;

            int i0 = near_i > 0 ? near_i - 1 : 0;
            int i1 = near_i;
            int i2 = near_i < r - 1 ? near_i + 1 : r - 1;
            int i3 = near_i < r - 2 ? near_i + 2 : r - 1;

            int j0 = near_j > 0 ? near_j - 1 : 0;
            int j1 = near_j;
            int j2 = near_j < c - 1 ? near_j + 1 : c - 1;
            int j3 = near_j < c - 2 ? near_j + 2 : c - 1;

            float p0 = hermite(inputImage->data[i0 * c + j0], (inputImage->data[i1 * c + j0] - inputImage->data[i0 * c + j0]) / 2.0, inputImage->data[i2 * c + j0], (inputImage->data[i2 * c + j0] - inputImage->data[i1 * c + j0]) / 2.0, dy);
            float p1 = hermite(inputImage->data[i0 * c + j1], (inputImage->data[i1 * c + j1] - inputImage->data[i0 * c + j1]) / 2.0, inputImage->data[i2 * c + j1], (inputImage->data[i2 * c + j1] - inputImage->data[i1 * c + j1]) / 2.0, dy);
            float p2 = hermite(inputImage->data[i0 * c + j2], (inputImage->data[i1 * c + j2] - inputImage->data[i0 * c + j2]) / 2.0, inputImage->data[i2 * c + j2], (inputImage->data[i2 * c + j2] - inputImage->data[i1 * c + j2]) / 2.0, dy);
            float p3 = hermite(inputImage->data[i0 * c + j3], (inputImage->data[i1 * c + j3] - inputImage->data[i0 * c + j3]) / 2.0, inputImage->data[i2 * c + j3], (inputImage->data[i2 * c + j3] - inputImage->data[i1 * c + j3]) / 2.0, dy);

            float result = hermite(p0, (p1 - p0) / 2.0, p2, (p2 - p1) / 2.0, dx);
            outputImage.data[i * outputImage.width + j] = (unsigned char)result;
        }
    }
    return outputImage;
}


Image nnbr(const Image *inputImage, float zoom) {
    Image outputImage;
    int r = inputImage->height;
    int c = inputImage->width;

    int intZoom = (int)  floor(zoom);
    // Calcul de la taille de l'image zoomée
    outputImage.width = c * intZoom;
    outputImage.height = r * intZoom;
    outputImage.channels = inputImage->channels;

    // Allocation de la mémoire pour l'image zoomée
    outputImage.data = (unsigned char *)malloc(outputImage.width * outputImage.height * outputImage.channels);
    
    if (outputImage.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    int near_i, near_j;
    for (int i = 0; i < outputImage.height; i++) {
        for (int j = 0; j < outputImage.width; j++) {
            near_i = (int)floor(i / zoom);
            near_j = (int)floor(j / zoom);
            outputImage.data[i * outputImage.width + j] = inputImage->data[near_i * c + near_j];
        }
    }

    return outputImage;
}
