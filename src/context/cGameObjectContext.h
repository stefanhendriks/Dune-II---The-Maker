#pragma once

#include <stdexcept>
#include <cstddef>
#include <memory>
#include "gameobjects/map/MapGeometry.hpp"

class cBullets;
class cPlayers;
class cParticles;
class cStructures;
class cUnits;
class cMap;
class cPlayer;
class cUnit;
class cAbstractStructure;
class cStructureFactory;

struct sGameServices;

class cGameObjectContext {
public:
    cGameObjectContext(
        std::unique_ptr<cBullets> bullets,
        std::unique_ptr<cPlayers> players,
        std::unique_ptr<cParticles> particles,
        std::unique_ptr<cStructures> structures,
        std::unique_ptr<cUnits> units,
        std::unique_ptr<cMap> map,
        std::unique_ptr<cStructureFactory> structureFactory);
    ~cGameObjectContext();

    cBullets& getBullets() const;
    cMap& getMap() const;
    MapGeometry* getMapGeometry() const;
    cPlayers* getPlayers() const;
    cParticles& getParticles() const;

    cStructures& getStructures() const;
    cAbstractStructure* getStructure(int index);

    cUnits* getUnits() const;
    int getUnitsSize() const;
    cUnit* getUnit(std::size_t index);
    cUnit* getUnit(int index);

    cPlayer* getPlayer(int index);
    const cPlayer* getPlayer(int index) const;

    cStructureFactory* getStructureFactory() const;

    void serviceInit(sGameServices* services);
private:
    std::unique_ptr<cBullets> m_Bullets;
    std::unique_ptr<cPlayers> m_Players;
    std::unique_ptr<cParticles> m_particles;
    std::unique_ptr<cStructures> m_pStructures;
    std::unique_ptr<cUnits> m_Units;
    std::unique_ptr<cMap> m_map;
    std::unique_ptr<cStructureFactory> m_structureFactory;
};
