#include "context/cInfoContext.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/cSpecialInfos.h"
#include "gameobjects/cUpgradeInfo.h"
#include "gameobjects/units/cUnitInfos.h"

cInfoContext::~cInfoContext() {
}

void cInfoContext::setParticleInfos(std::unique_ptr<cParticleInfos> particleInfos) {
    if (!particleInfos) {
        throw std::runtime_error("cParticleInfos is not initialized!");
    }
    m_particleInfos = std::move(particleInfos);
}

cParticleInfos* cInfoContext::getParticleInfos() const {
    return m_particleInfos.get();
}

void cInfoContext::setStructureInfos(std::unique_ptr<cStructureInfos> structureInfos) {
    if (!structureInfos) {
        throw std::runtime_error("cStructureInfos is not initialized!");
    }
    m_structureInfos = std::move(structureInfos);
}

cStructureInfos* cInfoContext::getStructureInfos() const {
    return m_structureInfos.get();
}

void cInfoContext::setBulletInfos(std::unique_ptr<cBulletInfos> bulletInfos) {
    if (!bulletInfos) {
        throw std::runtime_error("cBulletInfos is not initialized!");
    }
    m_bulletInfos = std::move(bulletInfos);
}

cBulletInfos* cInfoContext::getBulletInfos() const {
    return m_bulletInfos.get();
}

void cInfoContext::setSpecialInfos(std::unique_ptr<cSpecialInfos> specialInfos) {
    if (!specialInfos) {
        throw std::runtime_error("cSpecialInfos is not initialized!");
    }
    m_specialInfos = std::move(specialInfos);
}

cSpecialInfos* cInfoContext::getSpecialInfos() const {
    return m_specialInfos.get();
}

void cInfoContext::setUpgradeInfos(std::unique_ptr<cUpgradeInfos> upgradeInfos) {
    if (!upgradeInfos) {
        throw std::runtime_error("cUpgradeInfos is not initialized!");
    }
    m_upgradeInfos = std::move(upgradeInfos);
}

cUpgradeInfos* cInfoContext::getUpgradeInfos() const {
    return m_upgradeInfos.get();
}

void cInfoContext::setUnitInfos(std::unique_ptr<cUnitInfos> unitInfos) {
    if (!unitInfos) {
        throw std::runtime_error("cUnitInfos is not initialized!");
    }
    m_unitInfos = std::move(unitInfos);
}

cUnitInfos* cInfoContext::getUnitInfos() const {
    return m_unitInfos.get();
}

