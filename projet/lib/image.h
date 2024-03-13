#ifndef IMAGE_H
#define IMAGE_H


typedef struct Image{
    int width;
    int height;
    int channels; 
    unsigned char  *data;
    char* path; 
}Image;




/**
 * Primitives
*/
int getWidth(Image image);
int getHeight(Image image);
int getnbPixelChannels(Image image);
char* getData(Image image);
int* getPixel(Image image, int x, int y);
char* getImagePath(Image image);
Image loadImage(char* path);
Image freeImage(Image image);
void affichePixel(Image image, int x, int y);
// int writeImagePng(Image image);
// int writeImageJpg(Image image, int quality);



#endif