#pragma once

#include <SDL2/SDL.h>

class Texture;
class cPlayer;

// function to create a texture from an indexed surface, applying the player-specific palette.
Texture* createPlayerTextureFromIndexedSurfaceWithPalette(cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency);