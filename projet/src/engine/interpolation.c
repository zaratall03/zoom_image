#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "interpolation.h"
#include "image.h"

// Fonction d'interpolation pour le zoom Hermite
float interpolateHermite(float p0, float p1, float p2, float p3, float t) {
    float c0 = p1;
    float c1 = 0.5f * (p2 - p0);
    float c2 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    float c3 = 0.5f * (p3 - p0) + 1.5f * (p1 - p2);
    return ((c3 * t + c2) * t + c1) * t + c0;
}

// Algorithme de zoom Hermite avec paramètres de zoom et de coordonnées
Image zoomHermite(Image image, float zoomFactor, int centerX, int centerY) {
    printf("\nHermite\n");
    int newWidth = image.width;
    int newHeight = image.height;

    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = NULL;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            float srcX = (x - centerX) / zoomFactor + centerX;
            float srcY = (y - centerY) / zoomFactor + centerY;

            int srcX0 = (int)floorf(srcX);
            int srcX1 = (int)ceilf(srcX);
            int srcY0 = (int)floorf(srcY);
            int srcY1 = (int)ceilf(srcY);

            float u = srcX - srcX0;
            float v = srcY - srcY0;

            for (int c = 0; c < image.channels; c++) {
                float p00 = image.data[(srcY0 * image.width + srcX0) * image.channels + c];
                float p01 = image.data[(srcY1 * image.width + srcX0) * image.channels + c];
                float p10 = image.data[(srcY0 * image.width + srcX1) * image.channels + c];
                float p11 = image.data[(srcY1 * image.width + srcX1) * image.channels + c];

                float interpolated = interpolateHermite(p00, p01, p10, p11, v);
                newImage.data[(y * newWidth + x) * image.channels + c] = (unsigned char)interpolated;
            }
        }
    }

    return newImage;
}

// Algorithme de zoom plus proche voisin avec paramètres de zoom et de coordonnées
Image zoomNearestNeighbor(Image image, float zoomFactor, int centerX, int centerY) {
    printf("\nPlus proche voisin");
    int newWidth = image.width;
    int newHeight = image.height;

    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = NULL;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = roundf((x - centerX) / zoomFactor + centerX);
            int srcY = roundf((y - centerY) / zoomFactor + centerY);

            for (int c = 0; c < image.channels; c++) {
                newImage.data[(y * newWidth + x) * image.channels + c] =
                    image.data[(srcY * image.width + srcX) * image.channels + c];
            }
        }
    }

    return newImage;
}

// Algorithme de zoom bilinéaire avec paramètres de zoom et de coordonnées
Image zoomBilinear(Image image, float zoomFactor, int centerX, int centerY) {
    printf("\nBilineaire");
    int newWidth = image.width;
    int newHeight = image.height;

    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = NULL;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            float srcX = (x - centerX) / zoomFactor + centerX;
            float srcY = (y - centerY) / zoomFactor + centerY;

            int srcX0 = (int)floorf(srcX);
            int srcX1 = (int)ceilf(srcX);
            int srcY0 = (int)floorf(srcY);
            int srcY1 = (int)ceilf(srcY);

            float u = srcX - srcX0;
            float v = srcY - srcY0;

            for (int c = 0; c < image.channels; c++) {
                float p00 = image.data[(srcY0 * image.width + srcX0) * image.channels + c];
                float p01 = image.data[(srcY1 * image.width + srcX0) * image.channels + c];
                float p10 = image.data[(srcY0 * image.width + srcX1) * image.channels + c];
                float p11 = image.data[(srcY1 * image.width + srcX1) * image.channels + c];

                float interpolated = (1 - u) * (1 - v) * p00 + u * (1 - v) * p10 + (1 - u) * v * p01 + u * v * p11;
                newImage.data[(y * newWidth + x) * image.channels + c] = (unsigned char)interpolated;
            }
        }
    }

    return newImage;
}
