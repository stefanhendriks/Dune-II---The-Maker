#include "cGameObjectContextCreator.h"
#include "cGameObjectContext.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/units/cUnits.h"
#include "player/cPlayers.h"
#include "map/cMap.h"
#include "include/sGameServices.h"

std::unique_ptr<cGameObjectContext> cGameObjectsContextCreator::create(sGameServices* services)
{
    return std::make_unique<cGameObjectContext>(
        std::make_unique<cBullets>(),
        std::make_unique<cPlayers>(services),
        std::make_unique<cParticles>(),
        std::make_unique<cStructures>(),
        std::make_unique<cUnits>(),
        std::make_unique<cMap>(),
        std::make_unique<cStructureFactory>()
    );
}
