#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"
#include "image.h"

int main() {
    Image img  = loadImage("./ressources/img.jpeg");  
    int width = getWidth(img);  
    int height = getHeight(img); 
    int channels = getnbPixelChannels(img);
    printf("Chargement de l'image réussi.\n");
    printf("Dimensions de l'image : %d x %d\n", width, height);
    printf("Valeurs des pixels :\n");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            affichePixel(img, x, y);
        }
        printf("\n");
    }

    freeImage(img);
    return 0;
}
