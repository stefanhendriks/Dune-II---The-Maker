/**
 * @file cInfoContextCreator.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <memory>

class cParticleInfos;
class cStructureInfos;
class cBulletInfos;
class cSpecialInfos;
class cUpgradeInfos;
class cUnitInfos;
class cInfoContext;
class cGameSettings;
class Graphics;
class GraphicsContext;
struct SDL_Renderer;
struct s_TerrainInfo;

class cInfoContextCreator {
public:
    explicit cInfoContextCreator(GraphicsContext* graphicsContext);
    ~cInfoContextCreator() = default;

    std::unique_ptr<cParticleInfos> createParticleInfos();
    std::unique_ptr<cStructureInfos> createStructureInfos();
    std::unique_ptr<cBulletInfos> createBulletInfos();
    std::unique_ptr<cSpecialInfos> createSpecialInfos();
    std::unique_ptr<cUpgradeInfos> createUpgradeInfos(const cInfoContext* infoContext, const cGameSettings* settings);
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
    void initUpgrades(cUpgradeInfos& upgradeInfos, const cInfoContext* infoContext, const cGameSettings* settings);

    Graphics* m_gfxdata = nullptr;
};
