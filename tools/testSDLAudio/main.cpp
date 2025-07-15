#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

//g++ -o test main.cpp -lSDL2 -lSDL2_mixer

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    // Charger un effet sonore
    Mix_Chunk *son = Mix_LoadWAV("Sound_machineGun.wav");
    if (!son) {
        printf("Erreur Mix_LoadWAV: %s\n", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    // Charger une musique
    Mix_Music *musique = Mix_LoadMUS("Dune11_18.mid");
    if (!musique) {
        printf("Erreur Mix_LoadMUS: %s\n", Mix_GetError());
        Mix_FreeChunk(son);
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    int maxChannels = Mix_AllocateChannels(64);
    printf("Max channels : %i\n", maxChannels);

    // Jouer la musique en boucle
    Mix_PlayMusic(musique, -1);

    printf("Appuyez sur Entrée pour jouer le son...\n");
    getchar();
    Mix_PlayChannel(-1, son, 0);
    getchar();
    Mix_PlayChannel(-1, son, 0);
    getchar();
    Mix_PlayChannel(-1, son, 0);
    printf("Appuyez sur Entrée pour quitter...\n");
    getchar();

    // Nettoyage
    Mix_HaltMusic();
    Mix_FreeMusic(musique);
    Mix_FreeChunk(son);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}