#pragma once

#include <memory>

class cParticleInfos;
class cStructureInfos;
class cBulletInfos;
class cSpecialInfos;
class cUpgradeInfos;
class cUnitInfos;
class cInfoContext;
class SDL_Renderer;
struct s_TerrainInfo;

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
    std::unique_ptr<s_TerrainInfo> createTerrainInfos();

    void installInfos(cInfoContext& infoContext);

private:
    void installTerrain(s_TerrainInfo* terrainInfo);
    void initUnits(cUnitInfos& unitInfos);
    void initStructures(cStructureInfos& structureInfos);
    void initBullets(cBulletInfos& bulletInfos);
    void initSpecials(cSpecialInfos& specialInfos);
    void initParticles(cParticleInfos& particleInfos);
    void initUpgrades(cUpgradeInfos& upgradeInfos);
};
