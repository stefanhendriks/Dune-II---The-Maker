#include "cGameObjectContextCreator.h"
#include "cGameObjectContext.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "player/cPlayers.h"
#include "map/cMap.h"

std::unique_ptr<cBullets> cGameObjectsContextCreator::createBullets() {
    return std::make_unique<cBullets>();
}

std::unique_ptr<cPlayers> cGameObjectsContextCreator::createPlayers() {
    return std::make_unique<cPlayers>();
}

std::unique_ptr<cParticles> cGameObjectsContextCreator::createParticles() {
    return std::make_unique<cParticles>();
}

std::unique_ptr<cStructures> cGameObjectsContextCreator::createStructures() {
    return std::make_unique<cStructures>();
}

std::unique_ptr<cUnits> cGameObjectsContextCreator::createUnits() {
    return std::make_unique<cUnits>();
}

std::unique_ptr<cMap> cGameObjectsContextCreator::createMap() {
    return std::make_unique<cMap>();
}

void cGameObjectsContextCreator::installGameObjects(cGameObjectContext& gameObjectContext) {
    gameObjectContext.setBullets(createBullets());
    gameObjectContext.setPlayers(createPlayers());
    gameObjectContext.setParticles(createParticles());
    gameObjectContext.setStructures(createStructures());
    gameObjectContext.setUnits(createUnits());
    gameObjectContext.setMap(createMap());
}