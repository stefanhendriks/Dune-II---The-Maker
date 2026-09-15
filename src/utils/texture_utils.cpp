/**
 * @file texture_utils.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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