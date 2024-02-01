#include <stdio.h>
#include <stdlib.h>
#include "image.h"

int main() {
    Image img = loadImage("./ressources/imgjpeg.jpeg");
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

    img = freeImage(img);
    if(img.data == NULL){
        printf("\nOk c'est parfait");
    }else{
        printf("\nC'est la merde");
    }
    return 0;
}
