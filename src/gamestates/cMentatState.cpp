#include "gamestates/cMentatState.h"
#include "cGame.h"
#include "mentat/AtreidesMentat.h"
#include "mentat/HarkonnenMentat.h"
#include "mentat/OrdosMentat.h"
#include "mentat/BeneMentat.h"
#include "ini.h"
#include "include/d2tmc.h"
#include "include/iniDefine.h"
#include "utils/RNG.hpp"
#include "player/cPlayer.h"
#include "data/gfxdata.h"
#include "include/sDataCampaign.h"

cMentatState::cMentatState(cGame &game, GameContext* ctx, MentatMode mode, s_DataCampaign* dataCampaign, int house)
    : cGameState(game, ctx), m_dataCampaign(dataCampaign), m_mode(mode), m_house(house)
{
    prepareMentat();
}

cMentatState::~cMentatState()
{
    delete m_mentat;
}

eGameStateType cMentatState::getType()
{
    return eGameStateType::GAMESTATE_MENTAT;
}

void cMentatState::prepareMentat()
{
    int house = (m_house != -1) ? m_house : players[HUMAN].getHouse();
    bool allowMissionSelect = !m_game.m_skirmish; 
    delete m_mentat;
    switch (m_mode) {
        case MentatMode::Briefing:
            if (house == ATREIDES)
                m_mentat = new AtreidesMentat(m_ctx, allowMissionSelect);
            else if (house == HARKONNEN)
                m_mentat = new HarkonnenMentat(m_ctx, allowMissionSelect);
            else if (house == ORDOS)
                m_mentat = new OrdosMentat(m_ctx, allowMissionSelect);
            else 
                m_mentat = new BeneMentat(m_ctx, m_dataCampaign);
            m_game.missionInit();
            m_game.setupPlayers();
            cIni::loadScenario(house, m_dataCampaign->region, m_mentat, m_game.getReinforcements(),m_dataCampaign);
            cIni::loadBriefing(house, m_dataCampaign->region, INI_BRIEFING, m_mentat);
            break;
        case MentatMode::WinBrief:
            if (house == ATREIDES)
                m_mentat = new AtreidesMentat(m_ctx, true);
            else if (house == HARKONNEN)
                m_mentat = new HarkonnenMentat(m_ctx, true);
            else if (house == ORDOS) 
                m_mentat = new OrdosMentat(m_ctx, true);
            else 
                m_mentat = new BeneMentat(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("win01");
            else
                m_mentat->loadScene("win02");
            cIni::loadBriefing(house, m_dataCampaign->region, INI_WIN, m_mentat);
            break;
        case MentatMode::LoseBrief:
            if (house == ATREIDES) 
                m_mentat = new AtreidesMentat(m_ctx, true);
            else if (house == HARKONNEN)
                m_mentat = new HarkonnenMentat(m_ctx, true);
            else if (house == ORDOS)
                m_mentat = new OrdosMentat(m_ctx, true);
            else 
                m_mentat = new BeneMentat(m_ctx,m_dataCampaign);
            
            if (RNG::rnd(100) < 50)
                m_mentat->loadScene("lose01");
            else
                m_mentat->loadScene("lose02");
            
            cIni::loadBriefing(house, m_dataCampaign->region, INI_LOSE, m_mentat);
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
    m_game.getMouse()->setTile(MOUSE_NORMAL);
    if (m_mentat) m_mentat->draw();
    m_game.getMouse()->draw();
}

void cMentatState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mentat) m_mentat->onNotifyMouseEvent(event);
}

void cMentatState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_mentat) m_mentat->onNotifyKeyboardEvent(event);
}

void cMentatState::loadScenario(int iRegion, cReinforcements *reinforcements, s_DataCampaign *dataCampaign)
{
    cIni::loadScenario(m_dataCampaign->housePlayer, iRegion, m_mentat, reinforcements, dataCampaign);
}

void cMentatState::loadBriefing(int iScenarioFind, int iSectionFind)
{
    cIni::loadBriefing(m_dataCampaign->housePlayer, iScenarioFind, iSectionFind, m_mentat);
}