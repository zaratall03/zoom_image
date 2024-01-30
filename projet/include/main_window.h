#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <SDL2/SDL.h>
#include "main_window_const.h"

void destroy_window(SDL_Renderer* renderer, SDL_Window* window);
int init_window(SDL_Renderer* renderer, SDL_Window* window);
int routine(SDL_Renderer* renderer);
#endif