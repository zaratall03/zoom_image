#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h> //  fonctions mathématiques

void bilinear(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy, fx, fy, value;

    for (i = 0; i < r*zoom; i++) {
        for (j = 0; j < c*zoom; j++) {
            x = j/zoom;
            y = i/zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;
            ii = near_i*c;
            jj = near_j;
            fx = 1 - dx;
            fy = 1 - dy;
            value = fx*(fy*image[ii + jj] + dy*image[ii + jj + c]) +
                    dx*(fy*image[ii + jj + 1] + dy*image[ii + jj + c + 1]);
            imzoom[i*c*zoom + j] = (unsigned char)value;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    SDL_Surface *image, *imzoom;
    int zoom = 2; //
 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

 
    image = SDL_LoadBMP(argv[1]);

 
    if (image == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", argv[1], SDL_GetError());
        return 1;
    }


    int r = image->h;
    int c = image->w;


    imzoom = SDL_CreateRGBSurface(0, r*zoom, c*zoom, 24, 0, 0, 0, 0);

 
    bilinear(image->pixels, (unsigned char *)imzoom->pixels, r, c, zoom);


    SDL_Surface *screen;
    screen = SDL_SetVideoMode(r*zoom, c*zoom, 24, SDL_SWSURFACE);
    SDL_BlitSurface(imzoom, NULL, screen, NULL);
    SDL_Flip(screen);

    // ( 'q' ou ESC pour quitter)
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                    quit = 1;
                }
            }
        }
    }

    // Free memory
    SDL_FreeSurface(imzoom);
    SDL_FreeSurface(image);

    // Quit SDL
    SDL_Quit();

    return 0;
}
