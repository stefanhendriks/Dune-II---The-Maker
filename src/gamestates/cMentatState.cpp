#include "gamestates/cMentatState.h"
#include "mentat/AtreidesMentat.h"
#include "mentat/HarkonnenMentat.h"
#include "mentat/OrdosMentat.h"
#include "mentat/BeneMentat.h"
#include "context/GameContext.hpp"
#include "context/cGameObjectContext.h"
#include "game/cGameSettings.h"
#include "game/cGameInterface.h"
#include "controls/cMouse.h"
#include "utils/ini.h"
#include "include/iniDefine.h"
#include "utils/RNG.hpp"
#include "player/cPlayer.h"
#include "data/gfxdata.h"
#include "include/sDataCampaign.h"

#include <cassert>

cMentatState::cMentatState(sGameServices* services, MentatMode mode, s_DataCampaign* dataCampaign)
    : cGameState(services),
      m_dataCampaign(dataCampaign),
      m_settings(services->settings),
      m_interface(m_ctx->getGameInterface()),
      m_objets(services->objects),
      m_mode(mode),
      m_house(dataCampaign->housePlayer)
{
    assert(services != nullptr);
    assert(m_dataCampaign != nullptr);
    assert(m_settings != nullptr);
    assert(m_interface != nullptr);
    assert(m_objets != nullptr);
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
    auto& humanPlayer = m_objets->getPlayer(HUMAN);
    house = (m_house != -1) ? m_house : humanPlayer.getHouse();
    bool allowMissionSelect = !m_settings->isSkirmish();
    switch (m_mode) {
        case MentatMode::Briefing:
            if (house == ATREIDES)
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, allowMissionSelect);
            else if (house == ORDOS)
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx, m_dataCampaign);
            m_interface->missionInit();
            m_interface->setupPlayers();
            cIni::loadScenario(m_mentat.get(), m_interface->getReinforcements(),m_dataCampaign);
            cIni::loadBriefing(house, m_dataCampaign->region, INI_BRIEFING, m_mentat.get());
            break;
        case MentatMode::WinBrief:
            if (house == ATREIDES)
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, allowMissionSelect);
            else if (house == ORDOS) 
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("win01");
            else
                m_mentat->loadScene("win02");
            cIni::loadBriefing(house, m_dataCampaign->region, INI_WIN, m_mentat.get());
            break;
        case MentatMode::LoseBrief:
            if (house == ATREIDES) 
                m_mentat = std::make_unique<AtreidesMentat>(m_ctx, allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = std::make_unique<HarkonnenMentat>(m_ctx, allowMissionSelect);
            else if (house == ORDOS)
                m_mentat = std::make_unique<OrdosMentat>(m_ctx, allowMissionSelect);
            else 
                m_mentat = std::make_unique<BeneMentat>(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("lose01");
            else
                m_mentat->loadScene("lose02");
            
            cIni::loadBriefing(house, m_dataCampaign->region, INI_LOSE, m_mentat.get());
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
    cIni::loadScenario(m_mentat.get(), reinforcements, m_dataCampaign);
}

void cMentatState::loadBriefing(int iScenarioFind, int iSectionFind)
{
    cIni::loadBriefing(m_dataCampaign->housePlayer, iScenarioFind, iSectionFind, m_mentat.get());
}