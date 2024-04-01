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



/**
 * Effectue un zoom d'image en utilisant l'interpolation du voisin le plus proche.
 * 
 * @param image         L'image d'entrée à zoomer.
 * @param zoomFactor    Le facteur de zoom pour agrandir ou réduire l'image.
 * 
 * @return  L'image zoomée en utilisant l'algorithme du voisin le plus proche.
 */
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
Image zoomOutNearestNeighbor(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomNearestNeighbor(image, zoomFactorInverse);
}



/**
 * Calcule une valeur interpolée en fonction de quatre valeurs d'entrée et de deux facteurs d'interpolation.
 * 
 * @param p00   Valeur à la coordonnée (0, 0) de la grille d'interpolation.
 * @param p01   Valeur à la coordonnée (0, 1) de la grille d'interpolation.
 * @param p10   Valeur à la coordonnée (1, 0) de la grille d'interpolation.
 * @param p11   Valeur à la coordonnée (1, 1) de la grille d'interpolation.
 * @param u     Facteur d'interpolation horizontal, de 0 à 1.
 * @param v     Facteur d'interpolation vertical, de 0 à 1.
 * 
 * @return  La valeur interpolée aux coordonnées (u, v) basée sur les quatre valeurs d'entrée.
 */
float bilinearInterpolation(float p00, float p01, float p10, float p11, float u, float v) {
    return (1 - u) * (1 - v) * p00 + u * (1 - v) * p10 + (1 - u) * v * p01 + u * v * p11;
}

/**
 * Redimensionne une image en utilisant une interpolation bilinéaire avec un facteur de zoom spécifié.
 * 
 * @param image         L'image originale à redimensionner.
 * @param zoomFactor    Le facteur de zoom pour agrandir ou réduire l'image.
 * 
 * @return  L'image redimensionnée après interpolation bilinéaire.
 */
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



/**
 * Effectue une interpolation cubique sur un tableau de quatre valeurs avec le facteur d'interpolation spécifié.
 * 
 * @param p Les quatre points de contrôle pour l'interpolation cubique.
 * @param x Le facteur d'interpolation.
 * 
 * @return  La valeur interpolée à la position `x`.
 */
double cubicInterpolate(double p[4], double x) {
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

/**
 * Redimensionne une image en utilisant une interpolation bicubique avec un facteur de zoom spécifié.
 * 
 * @param image         L'image originale à redimensionner.
 * @param zoomFactor    Le facteur de zoom pour agrandir ou réduire l'image.
 * 
 * @return  L'image redimensionnée après interpolation bicubique.
 */
Image zoomBicubic(Image image, float zoomFactor) {
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image resizedImage;
    resizedImage.width = newWidth;
    resizedImage.height = newHeight;
    resizedImage.channels = image.channels;
    resizedImage.data = (unsigned char *)malloc(newWidth * newHeight * resizedImage.channels * sizeof(unsigned char));

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float origX = (float)x / zoomFactor;
            float origY = (float)y / zoomFactor;

            for (int c = 0; c < image.channels; ++c) {
                resizedImage.data[(y * newWidth + x) * resizedImage.channels + c] = bicubicInterpolate(&image, origX, origY, c);
            }
        }
    }

    return resizedImage;
}

/**
 * Effectue une interpolation bicubique sur une image à une position donnée.
 * 
 * @param image     Pointeur vers une structure représentant une image.
 * @param x         Coordonnée horizontale pour l'interpolation bicubique.
 * @param y         Coordonnée verticale pour l'interpolation bicubique.
 * @param channelIndex  Index du canal de couleur à interpoler dans l'image.
 * 
 * @return  La valeur interpolée bicubique pour les coordonnées (x, y) et l'indice du canal spécifié.
 */
double bicubicInterpolate(Image *image, double x, double y, int channelIndex) {
    int x_int = (int)x;
    int y_int = (int)y;
    double x_frac = x - x_int;
    double y_frac = y - y_int;

    double p[4][4];
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            int xi = x_int - 1 + i;
            int yj = y_int - 1 + j;
            xi = xi < 0 ? 0 : (xi >= image->width ? image->width - 1 : xi);
            yj = yj < 0 ? 0 : (yj >= image->height ? image->height - 1 : yj);
            p[j][i] = image->data[(yj * image->width + xi) * image->channels + channelIndex];
        }
    }

    double arr[4];
    arr[0] = cubicInterpolate(p[0], x_frac);
    arr[1] = cubicInterpolate(p[1], x_frac);
    arr[2] = cubicInterpolate(p[2], x_frac);
    arr[3] = cubicInterpolate(p[3], x_frac);
    return cubicInterpolate(arr, y_frac);
}


Image zoomOutBicubic(Image image, float zoomFactor) {
    float zoomFactorInverse = 1.0f / zoomFactor;
    return zoomBicubic(image, zoomFactorInverse);
}


/**
 * Utilise des verrous mutex pour accéder en toute sécurité et modifier les données dans la structure
 * partagée `resultTab` basée sur le type de zoom fourni.
 * 
 * @param type Le type de zoom utilisé pour identifier les résultats ou les images dans `resultTab`.
 * 
 * @return Les fonctions renvoient des valeurs de type `struct timespec` et `Image` en fonction du type de zoom.
 */
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


/**
 * Récupère le type de zoom actuellement affiché en assurant la sécurité des threads avec un mutex.
 * 
 * @return  Le type de zoom actuellement affiché, de type `ZoomType`.
 */
ZoomType getdisplayedZoomType(){
    pthread_mutex_lock(&lock);
    ZoomType res = displayedZoomType;
    pthread_mutex_unlock(&lock);
    return res;
}


/**
 * Définit le type de zoom affiché en toute sécurité des threads avec un mutex.
 * 
 * @param newType   Le type de zoom à afficher, de type `ZoomType`.
 */
void setdisplayedZoomType(ZoomType newType){
    pthread_mutex_lock(&lock);
    displayedZoomType =  newType;
    pthread_mutex_unlock(&lock);
}
/**
 * Calcule le temps écoulé en secondes entre deux structures timespec.
 * 
 * @param start Début de l'intervalle temporel.
 * @param end Fin de l'intervalle temporel.
 * 
 * @return  Le temps écoulé en secondes sous forme de valeur double.
 */
double calculateElapsedTime(struct timespec start, struct timespec end) {
    double elapsedSeconds = (double)(end.tv_sec - start.tv_sec); 
    double elapsedNanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
    double res = elapsedSeconds + elapsedNanoseconds;
    return res;
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


