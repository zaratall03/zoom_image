#include "painter.h"
#include "main_window_const.h"
#include <stdio.h>

// // Fonction pour dessiner une image SDL à partir de la structure Image
// void drawImage(SDL_Renderer* renderer, Image image) {
//     // Vérifiez que l'image est valide
//     printf("Je dessine l'image");
//     if (image.data == NULL) {
//         perror("Invalid image data\n");
//         return;
//     }

//     // Calculer la taille d'un pixel pour s'adapter à l'écran
//     int pixelSizeX = SCREEN_WIDTH / image.width;
//     int pixelSizeY = SCREEN_HEIGHT / image.height;
//     Uint8 r, g, b, a;
//     Uint8 rgba[4] = {r, g, b, a};
//     // Parcourir les pixels de l'image
//     for (int y = 0; y < image.height; y++) {
//         for (int x = 0; x < image.width; x++) {
//             // Récupérer les canaux de couleur du pixel
//             int* pixel = getPixel(image, x, y);
//             for(int channel=0; channel < 4; channel++){
//                 if(channel<image.channels){
//                     rgba[channel] = (Uint8)pixel[channel];
//                 }else{
//                     rgba[channel] = SDL_ALPHA_OPAQUE;
//                 }
//             }
//             printf("pix[%d,%d] {%d %d %d %d} ", x, y, r, g, b, a);
//             // Dessiner le pixel sur le renderer
//             SDL_SetRenderDrawColor(renderer, r, g, b, a);
//             SDL_Rect pixelRect = { x * pixelSizeX, y * pixelSizeY, pixelSizeX, pixelSizeY };
//             SDL_RenderFillRect(renderer, &pixelRect);
//         }
//         printf("\n");
//     }
// }

/**
 * The function `drawImage` takes an SDL renderer and an image as input, and draws the image on the
 * renderer by filling rectangles with the corresponding pixel colors.
 * 
 * @param renderer The `renderer` parameter is a pointer to an SDL_Renderer object. This renderer is
 * responsible for rendering the image on the screen.
 * @param image The "image" parameter is of type "Image" and represents the image data that needs to be
 * drawn.
 * 
 * @return The function does not return any value. It has a void return type.
 */
void drawImage(SDL_Renderer* renderer, Image image) {
    if (image.data == NULL) {
        perror("Invalid image data\n");
        return;
    }
    int pixelSizeX = SCREEN_WIDTH / image.width;
    int pixelSizeY = SCREEN_HEIGHT / image.height;
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            int* pixel = getPixel(image, x, y);
            if (pixel == NULL) {
                printf("Erreur lors de la récupération du pixel.\n");
                continue;
            }
            Uint8 rgba[4];
            for (int channel = 0; channel < 4; channel++) {
                if (channel < image.channels) {
                    rgba[channel] = (Uint8)pixel[channel];
                } else {
                    rgba[channel] = SDL_ALPHA_OPAQUE;
                }
            }
            SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
            SDL_Rect pixelRect = { x * pixelSizeX, y * pixelSizeY, pixelSizeX, pixelSizeY };
            SDL_RenderFillRect(renderer, &pixelRect);
            free(pixel);
        }
    }
}
