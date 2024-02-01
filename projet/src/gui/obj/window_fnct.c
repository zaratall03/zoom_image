#include "window_fnct.h"
#define WINDOW_TITLE "Projet zoom"
void destroy_window(SDL_Renderer* renderer, SDL_Window* window){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void init_window(SDL_Renderer** renderer, SDL_Window** window) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        exit(1);
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        exit(1);
    }
}

int routine(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}
