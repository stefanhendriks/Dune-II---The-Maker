#include "cGameObjectContext.h"
#include "include/sGameServices.h"

#include "gameobjects/particles/cParticles.h"
#include "gameobjects/projectiles/cBullets.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/units/cUnits.h"
#include "gameobjects/players/cPlayers.h"
#include "gameobjects/map/cMap.h"

#include <stdexcept>

cGameObjectContext::cGameObjectContext(
    std::unique_ptr<cBullets> bullets,
    std::unique_ptr<cPlayers> players,
    std::unique_ptr<cParticles> particles,
    std::unique_ptr<cStructures> structures,
    std::unique_ptr<cUnits> units,
    std::unique_ptr<cMap> map,
    std::unique_ptr<cStructureFactory> structureFactory)
    : m_Bullets(std::move(bullets))
    , m_Players(std::move(players))
    , m_particles(std::move(particles))
    , m_pStructures(std::move(structures))
    , m_Units(std::move(units))
    , m_map(std::move(map))
    , m_structureFactory(std::move(structureFactory))
{
    //hack: I didn't want to do, but I want to clear out all existing structures when creating a new context,
    // otherwise we might have leftover structures from previous games.
    // This is because the structure factory is shared across games, and it holds pointers to all existing structures,
    // so we need to clear them out when creating a new context because cMap has it
    m_structureFactory->deleteAllExistingStructures();
}

cGameObjectContext::~cGameObjectContext() = default;

cBullets& cGameObjectContext::getBullets() const {
    if (!m_Bullets) {
        throw std::runtime_error("cBullets not initialized in cGameObjectContext");
    }
    return *m_Bullets;
}

cPlayers* cGameObjectContext::getPlayers() const {
    if (!m_Players) {
        throw std::runtime_error("cPlayers not initialized in cGameObjectContext");
    }
    return m_Players.get();
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

cUnits* cGameObjectContext::getUnits() const {
    if (!m_Units) {
        throw std::runtime_error("cUnits not initialized in cGameObjectContext");
    }
    return m_Units.get();
}

int cGameObjectContext::getUnitsSize() const {
    if (!m_Units) {
        throw std::runtime_error("cUnits not initialized in cGameObjectContext");
    }
    return m_Units->size();
}

cMap& cGameObjectContext::getMap() const {
    if (!m_map) {
        throw std::runtime_error("cMap not initialized in cGameObjectContext");
    }
    return *m_map;
}

MapGeometry* cGameObjectContext::getMapGeometry() const {
    if (!m_map) {
        throw std::runtime_error("cMap not initialized in cGameObjectContext");
    }
    return &m_map->getGeometry();
}

cUnit* cGameObjectContext::getUnit(std::size_t index)
{
    if (!m_Units) return nullptr;
    if (index >= static_cast<std::size_t>(m_Units->size())) return nullptr;
    return &(*m_Units)[static_cast<int>(index)];
}

cUnit* cGameObjectContext::getUnit(int index)
{
    if (index < 0) return nullptr;
    return getUnit(static_cast<std::size_t>(index));
}

cAbstractStructure* cGameObjectContext::getStructure(int index)
{
    if (index < 0) return nullptr;

    auto &structures = getStructures();
    if (index >= static_cast<int>(structures.size())) return nullptr;

    return structures[index];
}

cPlayer* cGameObjectContext::getPlayer(int index)
{
    return m_Players->getPlayer(index);
}

const cPlayer* cGameObjectContext::getPlayer(int index) const
{
    return m_Players->getPlayer(index);
}

cStructureFactory* cGameObjectContext::getStructureFactory() const
{
    if (m_structureFactory == nullptr) {
        throw std::runtime_error("cStructureFactory not initialized in cGameObjectContext");
    }
    return m_structureFactory.get();
}

void cGameObjectContext::serviceInit(sGameServices* services)
{
    assert(m_Players != nullptr);
    m_Players->serviceInit(services);
    assert(m_map != nullptr);
    m_map->serviceInit(services);
    assert(m_Units != nullptr);
    m_Units->serviceInit(services);
}