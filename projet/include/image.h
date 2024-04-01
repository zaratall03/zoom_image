#ifndef IMAGE_H
#define IMAGE_H


typedef struct Image{
    int width;
    int height;
    int channels; 
    unsigned char  *data;
    char* path; 
    
}Image;




int getWidth(Image image);
int getHeight(Image image);
int getnbPixelChannels(Image image);
char* getData(Image image);
unsigned char* getPixel(Image image, int x, int y);
char* getImagePath(Image image);
void set_pixel(Image *img, int x, int y, int c, unsigned char value);
Image loadImage(char* path);
Image freeImage(Image image);
void affichePixel(Image image, int x, int y);
void free_image(Image *img);
unsigned char getPixelComposante(Image image, int x, int y, int channel);
void setPixelComposante(Image *image, int x, int y, int channel, unsigned char value);

void writeImageJpg(const char* filename, Image image); 



#endif