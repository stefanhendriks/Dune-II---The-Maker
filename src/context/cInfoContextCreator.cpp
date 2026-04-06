#include "context/cInfoContextCreator.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/cSpecialInfos.h"
#include "gameobjects/cUpgradeInfo.h"
#include "gameobjects/units/cUnitInfos.h"

cInfoContextCreator::cInfoContextCreator(SDL_Renderer *renderer) {
}

cInfoContextCreator::~cInfoContextCreator() {
}

std::unique_ptr<cParticleInfos> cInfoContextCreator::createcParticleInfos() {
    if (!m_particleInfos) {
        m_particleInfos = std::make_unique<cParticleInfos>();
    }
    return std::move(m_particleInfos);
}

std::unique_ptr<cStructureInfos> cInfoContextCreator::createStructureInfos() {
    if (!m_structureInfos) {
        m_structureInfos = std::make_unique<cStructureInfos>();
    }
    return std::move(m_structureInfos);
}

std::unique_ptr<cBulletInfos> cInfoContextCreator::createBulletInfos() {
    if (!m_bulletInfos) {
        m_bulletInfos = std::make_unique<cBulletInfos>();
    }
    return std::move(m_bulletInfos);
}

std::unique_ptr<cSpecialInfos> cInfoContextCreator::createSpecialInfos() {
    if (!m_specialInfos) {
        m_specialInfos = std::make_unique<cSpecialInfos>();
    }
    return std::move(m_specialInfos);
}

std::unique_ptr<cUpgradeInfos> cInfoContextCreator::createUpgradeInfos() {
    if (!m_upgradeInfos) {
        m_upgradeInfos = std::make_unique<cUpgradeInfos>();
    }
    return std::move(m_upgradeInfos);
}

std::unique_ptr<cUnitInfos> cInfoContextCreator::createUnitInfos() {
    if (!m_unitInfos) {
        m_unitInfos = std::make_unique<cUnitInfos>();
    }
    return std::move(m_unitInfos);
}
