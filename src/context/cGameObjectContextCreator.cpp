/**
 * @file cGameObjectContextCreator.cpp
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

#include "cGameObjectContextCreator.h"
#include "cGameObjectContext.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/units/cUnits.h"
#include "gameobjects/players/cPlayers.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/map/cPreviewMaps.h"

std::unique_ptr<cGameObjectContext> cGameObjectsContextCreator::create()
{
    return std::make_unique<cGameObjectContext>(
        std::make_unique<cBullets>(),
        std::make_unique<cPlayers>(),
        std::make_unique<cParticles>(),
        std::make_unique<cStructures>(),
        std::make_unique<cUnits>(),
        std::make_unique<cMap>(),
        std::make_unique<cPreviewMaps>(),
        std::make_unique<cStructureFactory>()
    );
}
