#pragma once

#include <memory>

class cParticleInfos;
class cStructureInfos;
class cBulletInfos;
class cSpecialInfos;
class cUpgradeInfos;
class cUnitInfos;
class SDL_Renderer;

class cInfoContextCreator {
public:
    cInfoContextCreator() = default;
    ~cInfoContextCreator() = default;

    std::unique_ptr<cParticleInfos> createParticleInfos();
    std::unique_ptr<cStructureInfos> createStructureInfos();
    std::unique_ptr<cBulletInfos> createBulletInfos();
    std::unique_ptr<cSpecialInfos> createSpecialInfos();
    std::unique_ptr<cUpgradeInfos> createUpgradeInfos();
    std::unique_ptr<cUnitInfos> createUnitInfos();

private:
    void initUnits(cUnitInfos& unitInfos);
    void initStructures(cStructureInfos& structureInfos);
    void initBullets(cBulletInfos& bulletInfos);
    void initSpecials(cSpecialInfos& specialInfos);
    void initParticles(cParticleInfos& particleInfos);
    void initUpgrades(cUpgradeInfos& upgradeInfos);
};
