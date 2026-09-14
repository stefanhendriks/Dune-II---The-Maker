/**
 * @file cPlayerStatistics.cpp
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

#include "cPlayerStatistics.h"

cPlayerStatistics::cPlayerStatistics()
{
    setEnemyStructuresDestroyed(0);
    setEnemyUnitsDestroyed(0);
    setSpiceMined(0);
    setStructuresConstructed(0);
    setStructuresDestroyed(0);
    setSuperWeaponsUsed(0);
    setUnitsConstructed(0);
    setUnitsEaten(0);
}
