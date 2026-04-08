#include "cGameObjectContext.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "player/cPlayers.h"
#include "map/cMap.h"

#include <stdexcept>

cGameObjectContext::cGameObjectContext(
    std::unique_ptr<cBullets> bullets,
    std::unique_ptr<cPlayers> players,
    std::unique_ptr<cParticles> particles,
    std::unique_ptr<cStructures> structures,
    std::unique_ptr<cUnits> units,
    std::unique_ptr<cMap> map)
    : m_Bullets(std::move(bullets))
    , m_Players(std::move(players))
    , m_particles(std::move(particles))
    , m_pStructures(std::move(structures))
    , m_Units(std::move(units))
    , m_map(std::move(map)) {
}

cGameObjectContext::~cGameObjectContext() = default;

cBullets& cGameObjectContext::getBullets() const {
    if (!m_Bullets) {
        throw std::runtime_error("cBullets not initialized in cGameObjectContext");
    }
    return *m_Bullets;
}

cPlayers& cGameObjectContext::getPlayers() const {
    if (!m_Players) {
        throw std::runtime_error("cPlayers not initialized in cGameObjectContext");
    }
    return *m_Players;
}

cParticles& cGameObjectContext::getParticles() const {
    if (!m_particles) {
        throw std::runtime_error("cParticles not initialized in cGameObjectContext");
    }
    return *m_particles;
}

cStructures& cGameObjectContext::getStructures() const {
    if (!m_pStructures) {
        throw std::runtime_error("cStructures not initialized in cGameObjectContext");
    }
    return *m_pStructures;
}

cUnits& cGameObjectContext::getUnits() const {
    if (!m_Units) {
        throw std::runtime_error("cUnits not initialized in cGameObjectContext");
    }
    return *m_Units;
}

cMap& cGameObjectContext::getMap() const {
    if (!m_map) {
        throw std::runtime_error("cMap not initialized in cGameObjectContext");
    }
    return *m_map;
}