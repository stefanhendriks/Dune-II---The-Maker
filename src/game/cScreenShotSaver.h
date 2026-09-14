/**
 * @file cScreenShotSaver.h
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

#pragma once
#include <SDL3/SDL.h>
#include <string>

class cScreenShotSaver {
public:
    static bool saveScreen(SDL_Renderer* renderer, int width, int height);
private:
    static unsigned int screenCount;
    static std::string getBaseFileName();
};