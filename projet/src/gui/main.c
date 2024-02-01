#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "window_fnct.h"
#include "painter.h"
#define NB_IMG  3
#define MAX_TIME_DISPLAYED 1
#include <SDL2/SDL.h>
#include <unistd.h>
#include "image.h"

int nextImageIndex(int currentIndex){
    return currentIndex < NB_IMG ? currentIndex+1 : 0;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    init_window(&renderer, &window);
    Image image = loadImage("./ressources/imgjpeg.jpeg");
    Image image1 = loadImage("./ressources/imgjpg.jpg");
    Image image2 = loadImage("./ressources/imgpng.png");
    Image imageToDisplay;
    int currentIndex = 0;
    Image images[NB_IMG] = {
        image,
        image1,
        image2
    };
    // Boucle principale
    int timeDisplayed = 0;
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        imageToDisplay = images[currentIndex];
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // reset
        SDL_RenderClear(renderer);
        drawImage(renderer, imageToDisplay); //draw
        SDL_RenderPresent(renderer); // affiche
        timeDisplayed++;
        currentIndex = timeDisplayed >= MAX_TIME_DISPLAYED ? currentIndex = nextImageIndex(currentIndex) : currentIndex; 
        sleep(1);
    }

    freeImage(image); 
    destroy_window(renderer, window);
    return 0;
}
