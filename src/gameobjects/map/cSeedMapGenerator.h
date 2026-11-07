/**
 * @file cSeedMapGenerator.h
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

#include "cSeedMap.h"

#include <array>

struct cell {
    short w, a, b;
};

// The generator works on 64 rows, but spreadMatrix may touch an extra 65th row.
using SeedCellMap = std::array<std::array<cell, 64>, 65>;

// 16x17 matrix, plus one extra element that noise may write to
using SeedMatrix = std::array<char, 16*17+1>;

class cSeedMapGenerator {

private:

    unsigned long seed;

    short random();	// create random number

    /* these is array used to find corresponding points */
    /*
          A.B.C    00..20..40
          .....    ..........
          D.E.F    02..22..42
          .....    ..........
          G.H.I    04..24..44
    */

    // methods needed to generate seedmap
    void convertMap (SeedCellMap &map, short *iconmap);
    void addSpice(SeedCellMap &map);
    void scanRegions (SeedCellMap &map);
    void createRegions (SeedCellMap &map);
    void balanceMap (SeedCellMap &map);
    void spreadMatrix (SeedCellMap &map);
    void copyMatrix (const SeedMatrix &matrix, SeedCellMap &map);
    void createMatrix (SeedMatrix &matrix);
    void addNoise1 (SeedMatrix &matrix);
    void addNoise2 (SeedMatrix &matrix);
    void addSpiceAt(SeedCellMap &map,short x,short y);

public:
    explicit cSeedMapGenerator(unsigned long value);

    cSeedMap generateSeedMap();
};
