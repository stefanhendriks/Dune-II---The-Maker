#include "texture_utils.h"
#include "gameobjects/players/cPlayer.h"
#include "drawers/SDLDrawer.hpp"

Texture* createPlayerTextureFromIndexedSurfaceWithPalette(SDLDrawer *renderer, cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency) {
    d2tm_assert(player && "player must be given");
    d2tm_assert(referenceSurface && "referenceSurface must be given");

    return renderer->createTextureFromIndexedSurfaceWithPalette(referenceSurface,
                                                                 paletteIndexForTransparency,
                                                                 player->getColors());
}