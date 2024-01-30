#include <stdio.h>
#include <stdlib.h>
#include "main_window.h"



int main(int argc, char *argv[]){
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;

    init_window(renderer, window);
    
    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        routine(renderer);
    }

    destroy_window(renderer, window); 

    return 0;
}
