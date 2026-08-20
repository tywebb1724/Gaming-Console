#include <SDL2/SDL.h>
#include <stdio.h>

int main(void) {
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
    int count = SDL_NumJoysticks();

    for (int i = 0; i < count; i++) {
        if (SDL_IsGameController(i)) {
            printf("%s\n", SDL_GameControllerNameForIndex(i));

            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                char* mapping = SDL_GameControllerMapping(controller);
                if (mapping) {
                    printf("%s\n", mapping);
                    SDL_free(mapping);
                } else {
                    printf("(no mapping string available)\n");
                }
                SDL_GameControllerClose(controller);
            } else {
                printf("(failed to open controller: %s)\n", SDL_GetError());
            }
        } else {
            //Not recognized as a standard game controller — no mapping string possible
            printf("%s\n", SDL_JoystickNameForIndex(i));
            printf("(not recognized as a game controller — no mapping available)\n");
        }
    }

    SDL_Quit();
    return 0;
}