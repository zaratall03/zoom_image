// #include "stb_image_write.h"
#include <stdlib.h>
#include "stb_image.h"
#include "image.h"


#define DEFAULT_QUALITY 95
#define MAX_CHANNELS 4

int getWidth(Image image){
    return image.width;
}
int getHeight(Image image){
    return image.height;
}

int getnbPixelChannels(Image image){
/**
 * @brief Renvoie le nombre de canaux dans une image.
 * @param image Le paramètre « image » est de type « Image ».
 * @return Le nombre de canaux dans l'image.
 */
    return image.channels;
}

char* getData(Image image){
/**
 * @brief La fonction renvoie le tableau de pixel de image.
 * @param image Le paramètre « image » est de type « Image ».
 * @return un pointeur vers le tableau de pixel
 */
    return image.data;
}

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




char* getImagePath(Image image){
    return image.path;
}

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
    /**
     * La fonction "affichePixel" imprime les valeurs des pixels à une position donnée dans une image.
     * 
     * @param image Le paramètre image est de type Image, qui est probablement une structure de données
     * représentant une image. Il peut contenir des informations telles que la largeur, la hauteur et
     * les pixels de l'image.
     * @param x Le paramètre x représente la coordonnée x du pixel dans l'image. Il spécifie la
     * position horizontale du pixel dans l'image.
     * @param y Le paramètre "y" représente la coordonnée y du pixel de l'image que l'on souhaite
     * afficher.
     */
    int* pixels = getPixel(image, x, y);
    int channels =  getnbPixelChannels(image);
    printf("[");
    for(int i=0; i<channels; i++){
        printf(" %d ", *(pixels+i)); 
    }
    printf("]");
}

Image freeImage(Image image){
    /**
     * @brief Libère l'image de la mémoire
     * @param image l'image à libérer
     * @return un image vide
    */
    stbi_image_free(image.data);
    Image img = {0, 0,0, NULL, NULL};
    return img;
}

// int writeImagePng(Image image){
//     /**
//      * La fonction écrit une image au format PNG.
//      * 
//      * @param image L'image à écrire
//      * @return une valeur entière.
//      */
//     return stbi_write_png(image.path, image.width, image.height, image.channels, image.data, image.channels * image.width);
// }


// int writeImageJpg(Image image, int quality){
//     /**
//      * La fonction écrit une image au format PNG.
//      * 
//      * @param image L'image à écrire
//      * @return une valeur entière.
//      */
//     return stbi_write_jpg(image.path, image.width, image.height, image.channels, image.data, DEFAULT_QUALITY);
// }

