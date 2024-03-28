
#include <stdlib.h>
#include "image.h"
#include "stb_image.h"  
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



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
    int subWidth = endX - startX;
    int subHeight = endY - startY;

    Image subImage;
    subImage.width = subWidth;
    subImage.height = subHeight;
    subImage.channels = image.channels;
    subImage.data = (unsigned char *)malloc(subWidth * subHeight * image.channels);
    subImage.path = NULL;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            for (int c = 0; c < image.channels; c++) {
                subImage.data[((y - startY) * subWidth + (x - startX)) * image.channels + c] = 
                    image.data[(y * image.width + x) * image.channels + c];
            }
        }
    }

    return subImage;
}

void free_image(Image *image) {
    if (image != NULL) {
        if (image->data != NULL) {
            free(image->data);
        }
        if (image->path != NULL){
            free(image->path);
        }
    }
}



unsigned char getPixelComposante(Image image, int x, int y, int channel) {
    int index = (y * image.width + x) * image.channels + channel;
    return image.data[index];
}

void setPixelComposante(Image *image, int x, int y, int channel, unsigned char value) {
    if (x < 0 || x >= image->width || y < 0 || y >= image->height || channel < 0 || channel >= image->channels) {
        fprintf(stderr, "SET Coordonnées de pixel invalides\n");
        exit(EXIT_FAILURE);
    }
    int index = (y * image->width + x) * image->channels + channel;
    image->data[index] = value;
}


void writeImageJpg(const char* filename, Image image){ 
    int result = stbi_write_png(filename, image.width, image.height, image.channels, image.data, image.width * image.channels);
    if (!result) {
        fprintf(stderr, "Erreur lors de l'écriture de l'image PNG : %s\n", filename);
        exit(EXIT_FAILURE);
    }
}
