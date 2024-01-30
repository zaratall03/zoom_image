


#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

void nnbr(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, near_i, near_j;
    for (i = 0; i < r * zoom; i++) {
        for (j = 0; j < c * zoom; j++) {
            near_i = (int)floor(i / zoom);
            near_j = (int)floor(j / zoom);
            imzoom[i * c * zoom + j] = image[near_i * c + near_j];
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    SDL_Surface *image, *imzoom;
    int zoom = 2; 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }


    image = SDL_LoadBMP(argv[1]);

    //Erreur
    if (image == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", argv[1], SDL_GetError());
        return 1;
    }

    //dimensions 
    int r = image->h;
    int c = image->w;

    // Allocation
    imzoom = SDL_CreateRGBSurface(0, r * zoom, c * zoom, 24, 0, 0, 0, 0);

    
    nnbr(image->pixels, (unsigned char *)imzoom->pixels, r, c, zoom);

 
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(r * zoom, c * zoom, 24, SDL_SWSURFACE);
    SDL_BlitSurface(imzoom, NULL, screen, NULL);
    SDL_Flip(screen);

    // key press
    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                goto cleanup;
            }
        }
    }

cleanup:
   
    SDL_FreeSurface(imzoom);
    SDL_FreeSurface(image);

    SDL_Quit();

    return 0;
}
