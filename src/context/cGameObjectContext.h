#pragma once

#include <stdexcept>
#include <memory>

class cBullets;
class cPlayers;
class cParticles;
class cStructures;
class cUnits;
class cMap;

class cGameObjectContext {
public:
    cGameObjectContext() = default;
    ~cGameObjectContext();

    void setBullets(std::unique_ptr<cBullets> bullets);
    cBullets& getBullets() const;

    void setPlayers(std::unique_ptr<cPlayers> players);
    cPlayers& getPlayers() const;

    void setParticles(std::unique_ptr<cParticles> particles);
    cParticles& getParticles() const;

    void setStructures(std::unique_ptr<cStructures> structures);
    cStructures& getStructures() const;

    void setUnits(std::unique_ptr<cUnits> units);
    cUnits& getUnits() const;

    void setMap(std::unique_ptr<cMap> map);
    cMap& getMap() const;

private:
    std::unique_ptr<cBullets> m_Bullets;
    std::unique_ptr<cPlayers> m_Players;
    std::unique_ptr<cParticles> m_particles;
    std::unique_ptr<cStructures> m_pStructures;
    std::unique_ptr<cUnits> m_Units;
    std::unique_ptr<cMap> m_map;
};
