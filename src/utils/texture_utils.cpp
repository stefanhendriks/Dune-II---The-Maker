#include "texture_utils.h"
#include "gameobjects/players/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "game/cGame.h"
#include "include/d2tmc.h"

Texture* createPlayerTextureFromIndexedSurfaceWithPalette(cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency) {
    assert(player && "player must be given");
    assert(referenceSurface && "referenceSurface must be given");
    int swapStart = player->getSwapColor();
    if (swapStart < 0) swapStart = -1;
    return global_renderDrawer->createTextureFromIndexedSurfaceWithPalette(referenceSurface, paletteIndexForTransparency, swapStart);
}