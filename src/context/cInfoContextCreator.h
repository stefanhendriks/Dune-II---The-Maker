/**
 * @file cInfoContextCreator.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <memory>
#include <string>

class cParticleInfos;
class cStructureInfos;
class cBulletInfos;
class cSpecialInfos;
class cUpgradeInfos;
class cUnitInfos;
class cInfoContext;
class cGameSettings;
class AudioContext;
class Graphics;
class GraphicsContext;
struct SDL_Renderer;
struct s_TerrainInfo;

class cInfoContextCreator {
public:
    // rulesDirectory holds the rules INI files (bullets.ini), it is the directory of the game rules file (houses.ini)
    cInfoContextCreator(GraphicsContext* graphicsContext, const AudioContext* audioContext, std::string rulesDirectory);
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
    void initSpecials(cSpecialInfos& specialInfos);
    void initParticles(cParticleInfos& particleInfos);
    void initUpgrades(cUpgradeInfos& upgradeInfos, const cInfoContext* infoContext, const cGameSettings* settings);

    Graphics* m_gfxdata = nullptr;
    Graphics* m_gfxinter = nullptr;
    const AudioContext* m_audio = nullptr;
    std::string m_rulesDirectory;
};
