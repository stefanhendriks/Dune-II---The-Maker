#pragma once

#include <SDL3/SDL.h>

class Texture;
class cPlayer;
class SDLDrawer;

// function to create a texture from an indexed surface, applying the player-specific palette.
Texture* createPlayerTextureFromIndexedSurfaceWithPalette(SDLDrawer* renderer, cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency);