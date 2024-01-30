#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>

float hermite(float p0, float m0, float p1, float m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float a = 2.0 * t3 - 3.0 * t2 + 1.0;
    float b = t3 - 2.0 * t2 + t;
    float c = -2.0 * t3 + 3.0 * t2;
    float d = t3 - t2;
    return a * p0 + b * m0 + c * p1 + d * m1;
}


void hermiteZoom(unsigned char *image, unsigned char *imzoom, int r, int c, int zoom) {
    int i, j, ii, jj, near_i, near_j;
    float x, y, dx, dy;

    for (i = 0; i < r * zoom; i++) {
        for (j = 0; j < c * zoom; j++) {
            x = j / (float)zoom;
            y = i / (float)zoom;
            near_i = (int)floor(y);
            near_j = (int)floor(x);
            dx = x - near_j;
            dy = y - near_i;

            int i0 = near_i > 0 ? near_i - 1 : 0;
            int i1 = near_i;
            int i2 = near_i < r - 1 ? near_i + 1 : r - 1;
            int i3 = near_i < r - 2 ? near_i + 2 : r - 1;

            int j0 = near_j > 0 ? near_j - 1 : 0;
            int j1 = near_j;
            int j2 = near_j < c - 1 ? near_j + 1 : c - 1;
            int j3 = near_j < c - 2 ? near_j + 2 : c - 1;

            float p0 = hermite(image[i0 * c + j0], (image[i1 * c + j0] - image[i0 * c + j0]) / 2.0, image[i2 * c + j0], (image[i2 * c + j0] - image[i1 * c + j0]) / 2.0, dy);
            float p1 = hermite(image[i0 * c + j1], (image[i1 * c + j1] - image[i0 * c + j1]) / 2.0, image[i2 * c + j1], (image[i2 * c + j1] - image[i1 * c + j1]) / 2.0, dy);
            float p2 = hermite(image[i0 * c + j2], (image[i1 * c + j2] - image[i0 * c + j2]) / 2.0, image[i2 * c + j2], (image[i2 * c + j2] - image[i1 * c + j2]) / 2.0, dy);
            float p3 = hermite(image[i0 * c + j3], (image[i1 * c + j3] - image[i0 * c + j3]) / 2.0, image[i2 * c + j3], (image[i2 * c + j3] - image[i1 * c + j3]) / 2.0, dy);

            float result = hermite(p0, (p1 - p0) / 2.0, p2, (p2 - p1) / 2.0, dx);
            imzoom[i * c * zoom + j] = (unsigned char)result;
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

  
    if (image == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", argv[1], SDL_GetError());
        return 1;
    }

    
    int r = image->h;
    int c = image->w;

    
    imzoom = SDL_CreateRGBSurface(0, r * zoom, c * zoom, 24, 0, 0, 0, 0);

  
    hermiteZoom(image->pixels, (unsigned char *)imzoom->pixels, r, c, zoom);

  
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(r * zoom, c * zoom, 24, SDL_SWSURFACE);
    SDL_BlitSurface(imzoom, NULL, screen, NULL);
    SDL_Flip(screen);

    
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

  
    SDL_FreeSurface(imzoom);
    SDL_FreeSurface(image);

 
    SDL_Quit();

    return 0;
}
