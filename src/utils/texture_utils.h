/**
 * @file texture_utils.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <SDL3/SDL.h>

class Texture;
class cPlayer;
class SDLDrawer;

// function to create a texture from an indexed surface, applying the player-specific palette.
Texture* createPlayerTextureFromIndexedSurfaceWithPalette(SDLDrawer* renderer, cPlayer* player, SDL_Surface* referenceSurface, int paletteIndexForTransparency);