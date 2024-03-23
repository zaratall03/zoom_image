// image.c

#include <stdlib.h>
#include "image.h"
#include "stb_image.h" // Ne pas inclure #define STB_IMAGE_IMPLEMENTATION

// Votre code pour manipuler les images ici


#define DEFAULT_QUALITY 95
#define MAX_CHANNELS 4

int getWidth(Image image){
    return image.width;
}
int getHeight(Image image){
    return image.height;
}

int getnbPixelChannels(Image image){
    return image.channels;
}

char* getData(Image image){
    return image.data;
}

char* getImagePath(Image image){
    return image.path;
}

/**
 * La fonction getPixel alloue de la mémoire pour un pixel dans une image et récupère les valeurs de
 * pixel pour les coordonnées spécifiées.
 * 
 * @param image La structure de données d'image contenant des informations sur l'image, telles que sa
 *              largeur, sa hauteur, le nombre de canaux de couleur et les données de pixels.
 * @param x     La coordonnée x du pixel dans l'image.
 * @param y     La coordonnée y du pixel dans l'image.
 * @return      Un pointeur vers un tableau d'entiers représentant les valeurs de pixels aux coordonnées
 *              spécifiées (x, y) dans l'image donnée.
 */
int* getPixel(Image image, int x, int y) {
    int* pixel = (int*)malloc(sizeof(int) * MAX_CHANNELS);
    if (pixel == NULL) {
        printf("Erreur lors de l'allocation de mémoire pour le pixel.\n");
        return NULL;
    } 
    for (int channel = 0; channel < image.channels; channel++) {
        int index = (y * image.width + x) * image.channels + channel;
        int valp = (int)image.data[index];
        pixel[channel] = abs(valp);
    }
    return pixel;
}

/**
 * La fonction loadImage charge une image à partir d'un chemin spécifié et renvoie une structure
 * Image contenant les données d'image, la largeur, la hauteur, les canaux et le chemin.
 * 
 * @param path Le chemin du fichier représentant l'image à charger.
 * @return     Une structure Image contenant les informations sur l'image chargée, telles que sa
 *             largeur, sa hauteur, le nombre de canaux, les données d'image et le chemin d'accès
 *             au fichier image.
 */
Image loadImage(char* path) {
    char *image_path = path;
    int width, height, channels;
    unsigned char *image_data = stbi_load(image_path, &width, &height, &channels, STBI_rgb);
    if (!image_data) {
        perror("Erreur lors du chargement de l'image.\n");
        exit(1);
    }
    
    Image img = {width, height, channels, image_data, image_path};
    return img;
}

void affichePixel(Image image, int x, int y){
    int* pixels = getPixel(image, x, y);
    int channels =  getnbPixelChannels(image);
    printf("[");
    for(int i=0; i<channels; i++){
        printf(" %d ", *(pixels+i)); 
    }
    printf("]");
}

Image freeImage(Image image){
    stbi_image_free(image.data);
    Image img = {0, 0,0, NULL, NULL};
    return img;
}


Image extractSubmatrix(Image image, int startX, int startY, int endX, int endY) {
    // Calcul des dimensions de la sous-matrice
    int subWidth = endX - startX;
    int subHeight = endY - startY;

    // Allocation de mémoire pour la sous-matrice extraite
    Image subImage;
    subImage.width = subWidth;
    subImage.height = subHeight;
    subImage.channels = image.channels;
    subImage.data = (unsigned char *)malloc(subWidth * subHeight * image.channels);
    subImage.path = NULL;

    // Parcours de chaque pixel de la sous-matrice
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            // Copie des valeurs de pixel correspondantes depuis l'image d'entrée vers la sous-matrice
            for (int c = 0; c < image.channels; c++) {
                subImage.data[((y - startY) * subWidth + (x - startX)) * image.channels + c] = 
                    image.data[(y * image.width + x) * image.channels + c];
            }
        }
    }

    return subImage;
}