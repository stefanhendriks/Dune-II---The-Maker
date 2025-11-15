#include "gamestates/cTellHouseState.h"
#include "cGame.h"
#include "mentat/AtreidesMentat.h"
#include "mentat/HarkonnenMentat.h"
#include "mentat/OrdosMentat.h"
#include "mentat/BeneMentat.h"
#include "ini.h"
#include "include/d2tmc.h"
#include "include/iniDefine.h"
#include "data/gfxdata.h"
#include "utils/RNG.hpp"
#include "player/cPlayer.h"

cTellHouseState::cTellHouseState(cGame &game, GameContext* ctx, int house)
    : cGameState(game, ctx), m_house(house)
{
    prepareMentat(house);
}

cTellHouseState::~cTellHouseState()
{
    delete m_mentat;
}

eGameStateType cTellHouseState::getType()
{
     return eGameStateType::GAMESTATE_TELLHOUSE; 
}

void cTellHouseState::prepareMentat(int house)
{
    m_house = house;
    delete m_mentat;
    m_mentat = new BeneMentat(m_ctx);
    m_mentat->setHouse(house);
    if (house == ATREIDES) {
        cIni::loadBriefing(ATREIDES, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("platr");
    } else if (house == HARKONNEN) {
        cIni::loadBriefing(HARKONNEN, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("plhar");
    } else if (house == ORDOS) {
        cIni::loadBriefing(ORDOS, 0, INI_DESCRIPTION, m_mentat);
        m_mentat->loadScene("plord");
    } else {
        m_mentat->setSentence(0, "Looks like you choose an unknown house");
    }
    m_mentat->speak();
}

void cTellHouseState::thinkFast()
{
    if (m_mentat) m_mentat->think();
}

void cTellHouseState::draw() const
{
    m_game.getMouse()->setTile(MOUSE_NORMAL);
    if (m_mentat) m_mentat->draw();
    m_game.getMouse()->draw();
}

void cTellHouseState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mentat) m_mentat->onNotifyMouseEvent(event);
}

void cTellHouseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_mentat) m_mentat->onNotifyKeyboardEvent(event);
}