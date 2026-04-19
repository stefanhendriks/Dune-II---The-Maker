#include "game/cGameInterface.h"
#include "data/gfxaudio.h"
#include "game/cGame.h"
#include "utils/cLog.h"
#include "include/Texture.hpp"
#include "controls/cMouse.h"
#include "gameobjects/units/cReinforcements.h"
#include "include/sGameEvent.h"

#include <format>

cGameInterface::cGameInterface(cGame* game)
    : m_igame(game)
{
}

cMouse* cGameInterface::getMouse() const
{
    return m_igame->getMouse();
}

void cGameInterface::prepareMentatToTellAboutHouse(int house) const
{
    switch(house) {
    case ATREIDES:
            m_igame->prepareMentatToTellAboutHouse(ATREIDES);
            m_igame->playSound(SOUND_ATREIDES);
            break;
    case HARKONNEN:
            m_igame->prepareMentatToTellAboutHouse(HARKONNEN);
            m_igame->playSound(SOUND_HARKONNEN);
            break;
    case ORDOS:
            m_igame->prepareMentatToTellAboutHouse(ORDOS);
            m_igame->playSound(SOUND_ORDOS);
            break;
    default:
        cLogger::getInstance()->log(LOG_WARN,COMP_GAMERULES, "House", std::format("cGameInterface::prepareMentatToTellAboutHouse() was called with an invalid house value: {}", house));
        break;
    }
    m_igame->setNextStateToTransitionTo(GAME_TELLHOUSE);
    m_igame->initiateFadingOut();
}

void cGameInterface::setTransitionToWithFadingOut(int newState) const
{
        m_igame->setNextStateToTransitionTo(newState);
        m_igame->initiateFadingOut();
}

void cGameInterface::initiateFadingOut() const
{
    m_igame->initiateFadingOut();
}

void cGameInterface::setNextStateToTransitionTo(int newState) const
{
    m_igame->setNextStateToTransitionTo(newState);
}

void cGameInterface::loadSkirmishMaps() const
{
    m_igame->loadSkirmishMaps();
}

void cGameInterface::initSkirmish() const
{
    m_igame->initSkirmish();
}

void cGameInterface::loadMapFromEditor(s_PreviewMap *map) const
{
    m_igame->loadMapFromEditor(map);
}

void cGameInterface::drawCursor() const
{
    m_igame->getMouse()->draw();
}

cDrawManager* cGameInterface::getDrawManager() const
{
    return m_igame->m_drawManager;
}

cMapCamera* cGameInterface::getMapCamera() const 
{
    return m_igame->m_mapCamera;
}

void cGameInterface::setPlayerToInteractFor(cPlayer *pPlayer) const
{
    m_igame->setPlayerToInteractFor(pPlayer);
}

cReinforcements* cGameInterface::getReinforcements() const
{
    return m_igame->getReinforcements();
}

void cGameInterface::setMissionWon() const
{
    m_igame->setMissionWon();
}

void cGameInterface::setMissionLost() const
{
    m_igame->setMissionLost();
}

void cGameInterface::missionInit() const
{
    m_igame->missionInit();
}

void cGameInterface::setupPlayers() const
{
    m_igame->setupPlayers();
}

void cGameInterface::onNotifyGameEvent(const s_GameEvent &event) const
{
    m_igame->onNotifyGameEvent(event);
}

void cGameInterface::checkMissionWinOrFail() const
{
    m_igame->checkMissionWinOrFail();
}

void cGameInterface::drawTextFps() const
{
    m_igame->drawTextFps();
}

void cGameInterface::drawTextTime() const
{
    m_igame->drawTextTime();
}

void cGameInterface::reduceShaking() const
{
    m_igame->reduceShaking();
}

void cGameInterface::goingToWinLoseBrief(int winOrLoseBrief) const
{
    m_igame->goingToWinLoseBrief(winOrLoseBrief);
}

Texture* cGameInterface::getScreenTexture() const
{
    return m_igame->getScreenTexture();
}

void cGameInterface::jumpToSelectYourNextConquestMission(int index) const
{
    m_igame->jumpToSelectYourNextConquestMission(index);
}

void cGameInterface::loadMapFromEditor(int iSkirmishMap) const
{
    m_igame->loadMapFromEditor(iSkirmishMap);
}

void cGameInterface::prepareMentatForPlayer() const
{
    m_igame->prepareMentatForPlayer();
}

void cGameInterface::loadScenario() const
{
    m_igame->loadScenario();
}

Color cGameInterface::getColorFadeSelected(int r, int g, int b) const
{
    return m_igame->getColorFadeSelected(r, g, b);
}

void cGameInterface::setWinFlags(int value) const
{
    m_igame->setWinFlags(value);
}

void cGameInterface::setLoseFlags(int value) const
{
    m_igame->setLoseFlags(value);
}

bool cGameInterface::playMusicByType(int iType) const
{
    return m_igame->playMusicByType(iType);
}

cDrawManager* cGameInterface::getRenderDrawManager() const
{
    return m_igame->m_drawManager;
}

int cGameInterface::getCurrentState() const
{
    return m_igame->getCurrentState();
}