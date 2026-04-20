#pragma once

#include <stdexcept>
#include <memory>

class cBullets;
class cPlayers;
class cParticles;
class cStructures;
class cUnits;
class cMap;
class cPlayer;
class cUnit;
class cStructureFactory;

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
    cPlayers* getPlayers() const;
    cParticles& getParticles() const;
    cStructures& getStructures() const;
    cUnits& getUnits() const;
    cMap& getMap() const;

    cPlayer* getPlayer(int index);
    const cPlayer* getPlayer(int index) const;
    cUnit& getUnit(int index);
    const cUnit& getUnit(int index) const;

    cStructureFactory* getStructureFactory() const;

private:
    std::unique_ptr<cBullets> m_Bullets;
    std::unique_ptr<cPlayers> m_Players;
    std::unique_ptr<cParticles> m_particles;
    std::unique_ptr<cStructures> m_pStructures;
    std::unique_ptr<cUnits> m_Units;
    std::unique_ptr<cMap> m_map;
    std::unique_ptr<cStructureFactory> m_structureFactory;
};
