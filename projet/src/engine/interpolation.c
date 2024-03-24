#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include "interpolation.h"
#include "image.h"
#include <pthread.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Fonction d'interpolation d'Hermite

extern ZoomType TYPE_ALGO; 
extern ResultTab resultTab;
extern pthread_mutex_t lock;


float hermiteInterpolation(float p00, float p01, float p10, float p11, float t) {
    float c0 = p00;
    float c1 = p01;
    float c2 = -3 * p00 + 3 * p01 - 2 * p10 - p11;
    float c3 = 2 * p00 - 2 * p01 + p10 + p11;

    return c0 + c1 * t + c2 * t * t + c3 * t * t * t;
}

// Fonction pour zoomer une image avec interpolation d'Hermite
Image zoomHermite(Image image, float zoomFactor) {
    int newWidth = (int)(image.width * zoomFactor);
    int newHeight = (int)(image.height * zoomFactor);

    Image outputImage;
    outputImage.width = newWidth;
    outputImage.height = newHeight;
    outputImage.channels = image.channels;
    outputImage.data = (unsigned char *)malloc(newWidth * newHeight * image.channels * sizeof(unsigned char));
    outputImage.path = NULL; // Le chemin n'est pas utilisé pour l'image zoomée

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
    calculateElapsedTime(res.start, res.end);
    printf("\n");
}

void afficheResultTab(ResultTab resultTab) {
    printf("Nombre d'algorithmes: %d\n", resultTab.nbAlgo);
    printf("Résultats:\n");
    for (int i = 0; i < resultTab.nbAlgo; ++i) {
        printf("Résultat %d:\n", i);
        afficheResult(resultTab.results[i]);
    }
}


void test1() {
    // Enregistrer le temps de début
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[RES_TEST1].start);
    pthread_mutex_unlock(&lock);
    
    // Faire dormir la fonction pendant un certain temps (par exemple 2 secondes)
    sleep(2);
    printf("Fini1");

    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[RES_TEST1].end);
    pthread_mutex_unlock(&lock);

}


void test2() {
    // Enregistrer le temps de début
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[RES_TEST2].start);
    pthread_mutex_unlock(&lock);
    // Faire dormir la fonction pendant un certain temps (par exemple 3 secondes)
    sleep(3);
    printf("Fini2");
    // Enregistrer le temps de fin
    pthread_mutex_lock(&lock);
    clock_gettime(CLOCK_MONOTONIC, &resultTab.results[RES_TEST2].end);
    pthread_mutex_unlock(&lock);

}


double calculateElapsedTime(struct timespec start, struct timespec end) {
// Fonction pour calculer le temps écoulé en secondes entre start et end
    double elapsedSeconds = (double)(end.tv_sec - start.tv_sec); // Calcul du nombre de secondes écoulées
    double elapsedNanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0; // Conversion des nanosecondes en secondes
    double res = elapsedSeconds + elapsedNanoseconds;
    printf("\n Temps mis par l'algo : %f", res );
    return res; // Temps total écoulé en secondes
}
