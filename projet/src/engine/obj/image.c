#include "image.h"
#include "stb_image.h"

/**
 * Primitives
 * width
height
channels
*data
*imagePath
*/
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
int* getPixel(Image image, int x, int y){
    int* pixel = (int*) malloc(sizeof(int)*image.channels);
    for(int channel = 0; channel< image.channels; channel++){
        pixel[channel] = image.data[image.width*x + y] * channel;
    }
    return pixel; 
}
char* getImagePath(Image image){
    return image.path;
}

/**
 * Opérations
*/
Image loadImage(char* path){
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
