
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

int main(int argc, char* args[]) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("Interface graphique avec bouton",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Boucle principale
    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            // Détecter les clics de souris
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Si le clic de souris est dans la zone du "bouton"
                if (e.button.x > 100 && e.button.x < 200 && e.button.y > 100 && e.button.y < 150) {
                    printf("Bouton cliqué!\n");
                }
            }
        }

        // Dessinez votre "bouton" ici (vous pouvez utiliser SDL_RenderDrawRect pour un contour)

        SDL_Delay(16); // Laissez un peu de temps pour éviter d'utiliser trop de ressources
    }

    // Libération des ressources
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
