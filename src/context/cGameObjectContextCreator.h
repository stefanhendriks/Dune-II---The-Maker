#pragma once

#include <memory>

class cGameObjectContext;
class cBullets;
class cPlayers;
class cParticles;
class cStructures;
class cUnits;
class cMap;

class cGameObjectsContextCreator {
public:
    cGameObjectsContextCreator() = default;
    ~cGameObjectsContextCreator() = default;

    std::unique_ptr<cBullets> createBullets();
    std::unique_ptr<cPlayers> createPlayers();
    std::unique_ptr<cParticles> createParticles();
    std::unique_ptr<cStructures> createStructures();
    std::unique_ptr<cUnits> createUnits();
    std::unique_ptr<cMap> createMap();

    void installGameObjects(cGameObjectContext& gameObjectContext);

private:

};
