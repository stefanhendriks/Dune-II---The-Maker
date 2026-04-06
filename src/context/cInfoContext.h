#pragma once

#include <stdexcept>
#include <memory>

#include "gameobjects/particles/cParticleInfos.h"
#include "gameobjects/units/cUnitInfos.h"
#include "gameobjects/structures/cStructureInfo.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include "gameobjects/cSpecialInfos.h"
#include "gameobjects/cUpgradeInfo.h"
#include "context/cInfoContextCreator.h"

class SDLDrawer;

class cInfoContext {
public:
    cInfoContext() = default;
    ~cInfoContext();

    void setParticleInfos(std::unique_ptr<cParticleInfos> particleInfos);
    cParticleInfos* getParticleInfos() const;
    void setStructureInfos(std::unique_ptr<cStructureInfos> structureInfos);
    cStructureInfos* getStructureInfos() const;
    void setBulletInfos(std::unique_ptr<cBulletInfos> bulletInfos);
    cBulletInfos* getBulletInfos() const;
    void setSpecialInfos(std::unique_ptr<cSpecialInfos> specialInfos);
    cSpecialInfos* getSpecialInfos() const;
    void setUpgradeInfos(std::unique_ptr<cUpgradeInfos> upgradeInfos);
    cUpgradeInfos* getUpgradeInfos() const;
    void setUnitInfos(std::unique_ptr<cUnitInfos> unitInfos);
    cUnitInfos* getUnitInfos() const;
private:
    std::unique_ptr<cParticleInfos> m_particleInfos;
    std::unique_ptr<cStructureInfos> m_structureInfos;
    std::unique_ptr<cBulletInfos> m_bulletInfos;
    std::unique_ptr<cSpecialInfos> m_specialInfos;
    std::unique_ptr<cUpgradeInfos> m_upgradeInfos;
    std::unique_ptr<cUnitInfos> m_unitInfos;
};