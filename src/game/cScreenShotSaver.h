#pragma once
#include <SDL2/SDL.h>
#include <string>

class cScreenShotSaver {
public:
    static bool saveScreen(SDL_Renderer* renderer, int width, int height);
private:
    static unsigned int screenCount;
    static std::string getBaseFileName();
};