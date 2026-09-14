/**
 * @file cRandomMapGenerator.h
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

#include "gameobjects/map/cPreviewMaps.h"

class cGameObjectContext;
class SDLDrawer;

class cRandomMapGenerator {
public:
    cRandomMapGenerator();
    void generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMapEntry, cGameObjectContext* objects, SDLDrawer* renderer);

private:
    //void drawProgress(float progress) const;
};
