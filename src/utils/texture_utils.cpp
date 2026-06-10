#include "texture_utils.h"
#include "gameobjects/players/cPlayer.h"
#include "drawers/SDLDrawer.hpp"

Texture* createPlayerTextureFromIndexedSurfaceWithPalette(SDLDrawer *renderer, cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency) {
    d2tm_assert(player && "player must be given");
    d2tm_assert(referenceSurface && "referenceSurface must be given");
    int swapStart = player->getSwapColor();
    if (swapStart < 0) swapStart = -1;
    return renderer->createTextureFromIndexedSurfaceWithPalette(referenceSurface, paletteIndexForTransparency, swapStart);
}