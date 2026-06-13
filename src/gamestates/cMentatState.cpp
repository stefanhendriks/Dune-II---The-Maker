#include "gamestates/cMentatState.h"
#include "gameobjects/mentat/AtreidesMentat.h"
#include "gameobjects/mentat/HarkonnenMentat.h"
#include "gameobjects/mentat/OrdosMentat.h"
#include "gameobjects/mentat/BeneMentat.h"
#include "gameobjects/mentat/SardaukarMentat.h"
#include "context/GameContext.hpp"
#include "context/cGameObjectContext.h"
#include "game/cGameSettings.h"
#include "game/cGameInterface.h"
#include "controls/cMouse.h"
#include "utils/ini.h"
#include "include/iniDefine.h"
#include "utils/RNG.hpp"
#include "gameobjects/players/cPlayer.h"
#include "data/gfxdata.h"
#include "include/sDataCampaign.h"

#include "include/cAssert.h"

cMentatState::cMentatState(sGameServices* services, MentatMode mode, cIni* cini, s_DataCampaign* dataCampaign)
    : cGameState(services),
      m_dataCampaign(dataCampaign),
      m_settings(services->settings),
      m_interface(m_ctx->getGameInterface()),
      m_objects(services->objects),
      m_cIni(cini),
      m_mode(mode),
      m_house(dataCampaign->housePlayer),
      m_allowMissionSelect(!m_settings->isSkirmish())
{
    d2tm_assert(services != nullptr);
    d2tm_assert(m_dataCampaign != nullptr);
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_interface != nullptr);
    d2tm_assert(m_objects != nullptr);
    d2tm_assert(m_cIni != nullptr);
    prepareMentat(m_house);
}

cMentatState::~cMentatState()
{}

eGameStateType cMentatState::getType()
{
    return eGameStateType::GAMESTATE_MENTAT;
}

void cMentatState::prepareMentat(int house)
{
    auto* humanPlayer = m_objects->getPlayer(HUMAN);
    house = (m_house > GENERALHOUSE) ? m_house : humanPlayer->getHouse();
    switch (m_mode) {
        case MentatMode::Briefing:
            if (house == ATREIDES)
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, m_allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, m_allowMissionSelect);
            else if (house == ORDOS)
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, m_allowMissionSelect);
            else if (house == SARDAUKAR)
                m_mentat = std::make_unique<SardaukarMentat>(m_ctx, m_allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx, m_dataCampaign);
            m_interface->missionInit();
            m_interface->setupPlayers();
            m_cIni->loadScenario(m_mentat.get(), m_interface->getReinforcements(),m_dataCampaign);
            m_cIni->loadBriefing(house, m_dataCampaign->region, INI_BRIEFING, m_mentat.get());
            break;
        case MentatMode::WinBrief:
            if (house == ATREIDES)
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, m_allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, m_allowMissionSelect);
            else if (house == ORDOS) 
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, m_allowMissionSelect);
            else if (house == SARDAUKAR)
                m_mentat = std::make_unique<SardaukarMentat>(m_ctx, m_allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("win01");
            else
                m_mentat->loadScene("win02");
            m_cIni->loadBriefing(house, m_dataCampaign->region, INI_WIN, m_mentat.get());
            break;
        case MentatMode::LoseBrief:
            if (house == ATREIDES) 
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, m_allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, m_allowMissionSelect);
            else if (house == ORDOS)
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, m_allowMissionSelect);
            else if (house == SARDAUKAR)
                m_mentat = std::make_unique<SardaukarMentat>(m_ctx, m_allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("lose01");
            else
                m_mentat->loadScene("lose02");
            
            m_cIni->loadBriefing(house, m_dataCampaign->region, INI_LOSE, m_mentat.get());
            break;
    }
    m_mentat->speak();
}

void cMentatState::thinkFast()
{
    if (m_mentat) m_mentat->think();
}

void cMentatState::draw() const
{
    m_interface->getMouse()->setTile(MOUSE_NORMAL);
    if (m_mentat) m_mentat->draw();
    m_interface->drawCursor();
}

void cMentatState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mentat) m_mentat->onNotifyMouseEvent(event);
}

void cMentatState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_mentat) m_mentat->onNotifyKeyboardEvent(event);
}

void cMentatState::loadScenario(cReinforcements *reinforcements)
{
    m_cIni->loadScenario(m_mentat.get(), reinforcements, m_dataCampaign);
}

void cMentatState::loadBriefing(int iScenarioFind, int iSectionFind)
{
    m_cIni->loadBriefing(m_dataCampaign->housePlayer, iScenarioFind, iSectionFind, m_mentat.get());
}