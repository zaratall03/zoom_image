#include "main_window.h"

void destroy_window(SDL_Renderer* renderer, SDL_Window* window){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int init_window(SDL_Renderer* renderer, SDL_Window* window){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    } 

    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get desktop display mode: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }else{
        dm.h = HEIGHT;
        dm.w = WIDTH;

    }

    if (SDL_CreateWindowAndRenderer(dm.w, dm.h, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }

    return 0; 
}

int routine(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

    // Charger l'image depuis un fichier avec stb_image.h
