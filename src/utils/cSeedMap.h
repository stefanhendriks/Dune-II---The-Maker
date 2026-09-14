/**
 * @file cSeedMap.h
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

#include <vector>
/**
 * Representation of terrain, called a seed map. This is not per-see generated from seed info. But
 * for now it is used to get the generated seed map into the Map class by D2TM.
 *
 */
class cSeedMap {
public:
    cSeedMap();

    static constexpr int kMapWidth = 64;
    static constexpr int kMapHeight = 64;

    void setCellType(int x, int y, int type);
    int getCellType(int x, int y);
    char getCellTypeCharacter(int x, int y);

private:
    std::vector<int> map; // 64x64
};
