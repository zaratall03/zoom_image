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


unsigned char* getPixel(Image image, int x, int y) {
    unsigned char * pixel = (unsigned char*)malloc(sizeof(unsigned char ) * image.channels);
    if (pixel == NULL) {
        printf("Erreur lors de l'allocation de mémoire pour le pixel.\n");
        return NULL;
    } 
    for (int channel = 0; channel < image.channels; channel++) {
        int index = (y * image.width + x) * image.channels + channel;
        pixel[channel] = abs(image.data[index]);
    }
    return pixel;
}



void set_pixel(Image *img, int x, int y, int c, unsigned char value) {
    img->data[(y * img->width + x) * img->channels + c] = value;
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
    unsigned char* pixels = getPixel(image, x, y);
    int channels =  getnbPixelChannels(image);
    printf("[");
    for(int i=0; i<channels; i++){
        printf(" %c ", *(pixels+i)); 
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