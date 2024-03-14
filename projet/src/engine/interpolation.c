#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpolation.h"

Image bilinear(const Image *inputImage, float zoom, int x, int y, int window_width, int window_height) {
    Image outputImage;

    // Calcule les coordonnées de l'angle supérieur gauche de la fenêtre de sortie
    int output_top_left_x = (int) (x - (window_width / (2 * zoom)));
    int output_top_left_y = (int) (y - (window_height / (2 * zoom)));

    // Calcule les coordonnées de l'angle inférieur droit de la fenêtre de sortie
    int output_bottom_right_x = (int) (x + (window_width / (2 * zoom)));
    int output_bottom_right_y = (int) (y + (window_height / (2 * zoom)));

    // Détermine les dimensions de outputImage
    outputImage.width = output_bottom_right_x - output_top_left_x;
    outputImage.height = output_bottom_right_y - output_top_left_y;
    outputImage.channels = inputImage->channels;

    // Allocation de la mémoire pour l'image zoomée
    outputImage.data = (unsigned char *)malloc(outputImage.width * outputImage.height * outputImage.channels * sizeof(unsigned char));
    if (outputImage.data == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    int i, j;
    float dx, dy, fx, fy;
    int near_i, near_j;
    int ii, jj;
    float value;

    for (i = 0; i < outputImage.height; i++) {
        for (j = 0; j < outputImage.width; j++) {
            float x_region = (float)output_top_left_x + (float)j / zoom;
            float y_region = (float)output_top_left_y + (float)i / zoom;

            near_i = (int)floor(y_region);
            near_j = (int)floor(x_region);

            dx = x_region - near_j;
            dy = y_region - near_i;

            fx = 1.0 - dx;
            fy = 1.0 - dy;

            for (int channel = 0; channel < inputImage->channels; channel++) {
                ii = near_i * inputImage->width * inputImage->channels;
                jj = near_j * inputImage->channels;

                value = fx * (fy * inputImage->data[ii + jj + channel] + dy * inputImage->data[ii + inputImage->channels + jj + channel]) +
                        dx * (fy * inputImage->data[ii + jj + inputImage->channels + channel] + dy * inputImage->data[ii + 2 * inputImage->channels + jj + channel]);

                outputImage.data[i * outputImage.width * outputImage.channels + j * outputImage.channels + channel] = (unsigned char)value;
            }
        }
    }

    printf("Fin du zoom\n");

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
