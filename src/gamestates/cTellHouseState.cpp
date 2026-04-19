#include "gamestates/cTellHouseState.h"
#include "mentat/BeneMentat.h"
#include "utils/ini.h"
#include "controls/cMouse.h"
#include "include/iniDefine.h"
#include "include/sDataCampaign.h"
#include "data/gfxdata.h"
#include "game/cGameInterface.h"
#include "context/GameContext.hpp"
#include "include/definitions.h"

#include <cassert>

cTellHouseState::cTellHouseState(sGameServices* services, s_DataCampaign* dataCampaign)
    : cGameState(services),
    m_house(dataCampaign->housePlayer),
    m_dataCampaign(dataCampaign)
{
    assert(services != nullptr);
    assert(dataCampaign != nullptr);
    auto interface = services->ctx->getGameInterface();
    m_mouse = interface->getMouse();
    assert(m_mouse != nullptr);
    prepareMentat(m_house);
}

cTellHouseState::~cTellHouseState()
{}

eGameStateType cTellHouseState::getType()
{
     return eGameStateType::GAMESTATE_TELLHOUSE; 
}

void cTellHouseState::prepareMentat(int house)
{
    m_house = house;
    m_mentat = std::make_unique<BeneMentat>(m_ctx, m_dataCampaign);
    m_mentat->setHouse(house);
    if (house == ATREIDES) {
        cIni::loadBriefing(ATREIDES, 0, INI_DESCRIPTION, m_mentat.get());
        m_mentat->loadScene("platr");
    } else if (house == HARKONNEN) {
        cIni::loadBriefing(HARKONNEN, 0, INI_DESCRIPTION, m_mentat.get());
        m_mentat->loadScene("plhar");
    } else if (house == ORDOS) {
        cIni::loadBriefing(ORDOS, 0, INI_DESCRIPTION, m_mentat.get());
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
    m_mouse->setTile(MOUSE_NORMAL);
    if (m_mentat) m_mentat->draw();
    m_mouse->draw();
}

void cTellHouseState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mentat) m_mentat->onNotifyMouseEvent(event);
}

void cTellHouseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_mentat) m_mentat->onNotifyKeyboardEvent(event);
}