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
