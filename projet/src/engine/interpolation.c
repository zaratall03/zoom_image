#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "interpolation.h"
#include "image.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


float hermiteInterpolation(float p0, float m0, float p1, float m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float a = 2.0 * t3 - 3.0 * t2 + 1.0;
    float b = t3 - 2.0 * t2 + t;
    float c = -2.0 * t3 + 3.0 * t2;
    float d = t3 - t2;
    return a * p0 + b * m0 + c * p1 + d * m1;
}

Image zoomHermite(Image image, float zoomFactor) {
    // Calcul des dimensions de la nouvelle image après zoom
    int newWidth = image.width * zoomFactor;
    int newHeight = image.height * zoomFactor;

    // Allocation de mémoire pour la nouvelle image
    Image newImage;
    newImage.width = newWidth;
    newImage.height = newHeight;
    newImage.channels = image.channels;
    newImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels);
    newImage.path = NULL;

    // Parcours de chaque pixel de la nouvelle image
    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // Calcul des coordonnées dans l'image d'entrée correspondant à ce pixel de la nouvelle image
            float srcX = (float)x / zoomFactor;
            float srcY = (float)y / zoomFactor;

            // Récupération des indices des pixels environnants dans l'image d'entrée
            int srcX0 = (int)(srcX);
            int srcX1 = srcX0 + 1;
            int srcY0 = (int)(srcY);
            int srcY1 = srcY0 + 1;

            // Calcul des valeurs de u et v pour l'interpolation Hermite
            float u = srcX - srcX0;
            float v = srcY - srcY0;

            // Parcours de chaque canal de couleur
            for (int c = 0; c < image.channels; c++) {
                // Récupération des valeurs de pixel environnants dans l'image d'entrée
                float p00 = image.data[(srcY0 * image.width + srcX0) * image.channels + c];
                float p01 = image.data[(srcY1 * image.width + srcX0) * image.channels + c];
                float p10 = image.data[(srcY0 * image.width + srcX1) * image.channels + c];
                float p11 = image.data[(srcY1 * image.width + srcX1) * image.channels + c];

                // Interpolation Hermite pour obtenir la valeur de pixel résultante
                float interpolated = hermiteInterpolation(p00, p01, p10, p11, v);

                // Attribution de la valeur de pixel résultante à la nouvelle image
                newImage.data[(y * newWidth + x) * image.channels + c] = (unsigned char)interpolated;
            }
        }
    }

    // Retour de la nouvelle image zoomée
    return newImage;
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
    newImage.path = NULL;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // Calcul des coordonnées correspondantes dans l'image originale
            int srcX = (int)(x / zoomFactor);
            int srcY = (int)(y / zoomFactor);

            // Copie de la valeur de pixel correspondante dans la nouvelle image
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
        // Gestion de l'échec de l'allocation mémoire
        fprintf(stderr, "Erreur : Échec de l'allocation de mémoire pour la nouvelle image.\n");
        exit(EXIT_FAILURE);
    }

    // Parcours de chaque pixel de la nouvelle image
    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            // Calcul des coordonnées correspondantes dans l'image originale
            float srcX = x / zoomFactor;
            float srcY = y / zoomFactor;

            // Récupération des coordonnées des pixels environnants dans l'image d'origine
            int srcX0 = (int)floor(srcX);
            int srcX1 = (int)ceil(srcX);
            int srcY0 = (int)floor(srcY);
            int srcY1 = (int)ceil(srcY);

            // Assurez-vous que les indices restent dans les limites de l'image d'origine
            srcX0 = CLAMP(srcX0, 0, image.width - 1);
            srcX1 = CLAMP(srcX1, 0, image.width - 1);
            srcY0 = CLAMP(srcY0, 0, image.height - 1);
            srcY1 = CLAMP(srcY1, 0, image.height - 1);

            // Calcul des poids de l'interpolation bilinéaire
            float u = srcX - srcX0;
            float v = srcY - srcY0;

            // Interpolation bilinéaire pour chaque canal de couleur
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



// Fonction de zoom arrière bilinéaire
Image zoomOutBilinear(Image image, float zoomFactor) {
        // Inverser le facteur de zoom pour le dézoom
    float zoomFactorInverse = 1.0f / zoomFactor;
    // Appeler la fonction de zoom Hermite avec le facteur de zoom inversé
    return zoomBilinear(image, zoomFactorInverse);
}



Image zoomOutHermite(Image image, float zoomFactor) {
        // Inverser le facteur de zoom pour le dézoom
    float zoomFactorInverse = 1.0f / zoomFactor;
    // Appeler la fonction de zoom Hermite avec le facteur de zoom inversé
    return zoomHermite(image, zoomFactorInverse);
}


