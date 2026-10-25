/**
 * @file cRandomMapGenerator.h
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

#include "gameobjects/map/PreviewMap.hpp"

class cGameObjectContext;

class cRandomMapGenerator {
public:
    cRandomMapGenerator();
    void generateRandomMap(int width, int height, int startingPoints, s_PreviewMap &randomMap, cGameObjectContext* objects);

private:
    //void drawProgress(float progress) const;
};
