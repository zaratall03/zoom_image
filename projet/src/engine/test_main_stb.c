#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"

int main() {
    const char *image_path = "./ressources/img.jpeg";
    int width, height, channels;
    unsigned char *image_data = stbi_load(image_path, &width, &height, &channels, STBI_rgb);
    
    if (!image_data) {
        perror("Erreur lors du chargement de l'image.\n");
        exit(1);
    } else {
        printf("Chargement de l'image réussi.\n");
        printf("Dimensions de l'image : %d x %d\n", width, height);
        printf("Valeurs des pixels :\n");
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int pixel_index = (y * width + x) * channels;
                printf("[");
                for (int c = 0; c < channels; c++) {
                    printf(" %d ", image_data[pixel_index + c]);
                }
                printf("]");
            }
            printf("\n");
        }
    }

    stbi_image_free(image_data);
    return 0;
}
