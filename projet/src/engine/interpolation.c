#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "interpolation.h"
#include "image.h"

/**
 * La fonction interpolateHermite calcule une valeur d'interpolation Hermite basée sur quatre points de
 * contrôle et un paramètre t.
 * 
 * @param p0 Valeur au début du segment d'interpolation.
 * @param p1 Point central autour duquel nous interpolons.
 * @param p2 Deuxième point de contrôle dans l'interpolation Hermite. Utilisé avec les autres points de
 *           contrôle (p0, p1, p3) pour calculer la valeur interpolée.
 * @param p3 Quatrième point de contrôle dans l'interpolation Hermite. Utilisé pour calculer la valeur
 *           interpolée en un point donné t.
 * @param t  Facteur d'interpolation entre les points p1 et p2. Typiquement une valeur entre 0 et 1,
 *           où 0 correspond au point p1 et 1 correspond au point p2.
 * 
 * @return   La valeur interpolée au temps t en utilisant l'interpolation Hermite basée sur les points
 *           de contrôle p0, p1, p2 et p3.
 */
float interpolateHermite(float p0, float p1, float p2, float p3, float t) {
    float c0 = p1;
    float c1 = 0.5f * (p2 - p0);
    float c2 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    float c3 = 0.5f * (p3 - p0) + 1.5f * (p1 - p2);
    return ((c3 * t + c2) * t + c1) * t + c0;
}

/**
 * La fonction « zoomHermite » prend une image, un facteur de zoom et des coordonnées centrales, et
 * effectue un zoom en utilisant l'interpolation Hermite.
 * 
 * @param image      L'image d'entrée sur laquelle appliquer le zoom à l'aide de l'interpolation Hermite.
 * @param zoomFactor Le facteur par lequel l'image sera zoomée ou dézoomée. Un zoomFactor supérieur à 1
 *                   agrandira l'image, tandis qu'un zoomFactor inférieur à 1 la réduira.
 * @param centerX    La coordonnée x du point central autour duquel l'image sera zoomée ou dézoomée. Ce point
 *                   sert de pivot pour l'opération de zoom, l'image s'agrandissant ou se contractant par
 *                   rapport à ce point central.
 * @param centerY    La coordonnée y du point central autour duquel l'image sera zoomée ou dézoomée. Ce point
 *                   sert de pivot pour l'opération de zoom, garantissant que l'image est mise à l'échelle par
 *                   rapport à ce point central.
 * @return           Une nouvelle image qui a été zoomée à l'aide de l'interpolation Hermite.
 */

Image zoomHermite(Image image, float zoomFactor, int centerX, int centerY) {
    printf("\nHermite");
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
Image zoomOutHermite(Image image, float zoomFactor, int centerX, int centerY) {
    // Inverser le facteur de zoom pour le dézoom
    float zoomFactorInverse = 1.0f / zoomFactor;
    // Appeler la fonction de zoom Hermite avec le facteur de zoom inversé
    return zoomHermite(image, zoomFactorInverse, centerX, centerY);
}

/**
 * La fonction zoomNearestNeighbor redimensionne une image à l'aide de l'algorithme du voisin le plus
 * proche basé sur un facteur de zoom et des coordonnées centrales spécifiés.
 * 
 * @param image      L'image originale à zoomer en utilisant l'algorithme du voisin le plus proche.
 * @param zoomFactor Le facteur par lequel l'image sera zoomée ou dézoomée. Un zoomFactor supérieur à 1
 *                   agrandira l'image, tandis qu'un zoomFactor inférieur à 1 la réduira.
 * @param centerX    La coordonnée x du point central autour duquel l'image sera zoomée ou dézoomée. Ce point
 *                   sert de référence pour déterminer comment l'image doit être mise à l'échelle en fonction
 *                   du zoomFactor.
 * @param centerY    La coordonnée y du point central autour duquel l'image sera zoomée ou dézoomée. Ce point
 *                   sert de référence pour déterminer comment l'image doit être mise à l'échelle en fonction
 *                   du zoomFactor.
 * @return           Une nouvelle image qui a été zoomée à l'aide de l'algorithme du voisin le plus proche.
 */

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
Image zoomOutNearestNeighbor(Image image, float zoomFactor, int centerX, int centerY) {
    // Inverser le facteur de zoom pour le dézoom
    float zoomFactorInverse = 1.0f / zoomFactor;
    // Appeler la fonction de zoom du voisin le plus proche avec le facteur de zoom inversé
    return zoomNearestNeighbor(image, zoomFactorInverse, centerX, centerY);
}




/**
 * La fonction `zoomBilinear` effectue une interpolation bilinéaire pour zoomer une image d'un facteur
 * spécifié autour d'un point central donné.
 * 
 * @param image      L'image d'entrée à zoomer. Contient des informations sur la largeur, la hauteur, les canaux et
 *                   les pixels de l'image.
 * @param zoomFactor Le facteur de zoom. Un facteur supérieur à 1 agrandira l'image, tandis qu'un facteur inférieur à 1
 *                   la réduira.
 * @param centerX    La coordonnée x du point central autour duquel l'image sera zoomée ou dézoomée. L'image s'agrandit
 *                   ou se contracte par rapport à ce point.
 * @param centerY    La coordonnée y du point central autour duquel l'image sera zoomée ou dézoomée. L'image s'agrandit
 *                   ou se contracte par rapport à ce point.
 * @return           Un nouvel objet Image qui a été zoomé à l'aide d'une interpolation bilinéaire basée sur les paramètres
 *                   d'entrée et les données de l'image d'origine.
 */

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
Image zoomOutBilinear(Image image, float zoomFactor, int centerX, int centerY) {
    // Inverser le facteur de zoom pour le dézoom
    float zoomFactorInverse = 1.0f / zoomFactor;
    // Appeler la fonction de zoom bilinéaire avec le facteur de zoom inversé
    return zoomBilinear(image, zoomFactorInverse, centerX, centerY);
}
