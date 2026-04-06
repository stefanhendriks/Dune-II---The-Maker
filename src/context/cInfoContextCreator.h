#pragma once

#include <memory>

class cParticleInfos;
class cStructureInfos;
class cBulletInfos;
class cSpecialInfos;
class cUpgradeInfos;
class SDL_Renderer;

class cInfoContextCreator {
public:
    cInfoContextCreator(SDL_Renderer *renderer);
    cInfoContextCreator() = delete;
    ~cInfoContextCreator();

    std::unique_ptr<cParticleInfos> createcParticleInfos();
    std::unique_ptr<cStructureInfos> createStructureInfos();
    std::unique_ptr<cBulletInfos> createBulletInfos();
    std::unique_ptr<cSpecialInfos> createSpecialInfos();
    std::unique_ptr<cUpgradeInfos> createUpgradeInfos();

private:
    std::unique_ptr<cParticleInfos> m_particleInfos;
    std::unique_ptr<cStructureInfos> m_structureInfos;
    std::unique_ptr<cBulletInfos> m_bulletInfos;
    std::unique_ptr<cSpecialInfos> m_specialInfos;
    std::unique_ptr<cUpgradeInfos> m_upgradeInfos;
};
