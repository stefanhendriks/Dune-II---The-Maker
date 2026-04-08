#include "cGameObjectContext.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "player/cPlayers.h"
#include "map/cMap.h"

#include <stdexcept>

cGameObjectContext::~cGameObjectContext() = default;

void cGameObjectContext::setBullets(std::unique_ptr<cBullets> bullets) {
    m_Bullets = std::move(bullets);
}

cBullets& cGameObjectContext::getBullets() const {
    if (!m_Bullets) {
        throw std::runtime_error("cBullets not initialized in cGameObjectContext");
    }
    return *m_Bullets;
}

void cGameObjectContext::setPlayers(std::unique_ptr<cPlayers> players) {
    m_Players = std::move(players);
}

cPlayers& cGameObjectContext::getPlayers() const {
    if (!m_Players) {
        throw std::runtime_error("cPlayers not initialized in cGameObjectContext");
    }
    return *m_Players;
}

void cGameObjectContext::setParticles(std::unique_ptr<cParticles> particles) {
    m_particles = std::move(particles);
}

cParticles& cGameObjectContext::getParticles() const {
    if (!m_particles) {
        throw std::runtime_error("cParticles not initialized in cGameObjectContext");
    }
    return *m_particles;
}

void cGameObjectContext::setStructures(std::unique_ptr<cStructures> structures) {
    m_pStructures = std::move(structures);
}

cStructures& cGameObjectContext::getStructures() const {
    if (!m_pStructures) {
        throw std::runtime_error("cStructures not initialized in cGameObjectContext");
    }
    return *m_pStructures;
}

void cGameObjectContext::setUnits(std::unique_ptr<cUnits> units) {
    m_Units = std::move(units);
}

cUnits& cGameObjectContext::getUnits() const {
    if (!m_Units) {
        throw std::runtime_error("cUnits not initialized in cGameObjectContext");
    }
    return *m_Units;
}

void cGameObjectContext::setMap(std::unique_ptr<cMap> map) {
    m_map = std::move(map);
}

cMap& cGameObjectContext::getMap() const {
    if (!m_map) {
        throw std::runtime_error("cMap not initialized in cGameObjectContext");
    }
    return *m_map;
}