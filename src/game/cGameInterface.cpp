#include "game/cGameInterface.h"
#include "data/gfxaudio.h"
#include "game/cGame.h"
#include "utils/cLog.h"
#include "include/Texture.hpp"
#include "controls/cMouse.h"
#include "gameobjects/units/cReinforcements.h"
#include "include/sGameEvent.h"
#include "utils/cStructureUtils.h"

#include <format>

cGameInterface::cGameInterface(cGame* game)
    : m_game(game)
{
}

cMouse* cGameInterface::getMouse() const
{
    return m_game->getMouse();
}

void cGameInterface::prepareMentatToTellAboutHouse(int house) const
{
    switch(house) {
    case ATREIDES:
            m_game->prepareMentatToTellAboutHouse(ATREIDES);
            m_game->playSound(SOUND_ATREIDES);
            break;
    case HARKONNEN:
            m_game->prepareMentatToTellAboutHouse(HARKONNEN);
            m_game->playSound(SOUND_HARKONNEN);
            break;
    case ORDOS:
            m_game->prepareMentatToTellAboutHouse(ORDOS);
            m_game->playSound(SOUND_ORDOS);
            break;
    default:
        cLogger::getInstance()->log(LOG_WARN,COMP_GAMERULES, "House", std::format("cGameInterface::prepareMentatToTellAboutHouse() was called with an invalid house value: {}", house));
        break;
    }
    m_game->setNextStateToTransitionTo(GAME_TELLHOUSE);
    m_game->initiateFadingOut();
}

void cGameInterface::setTransitionToWithFadingOut(int newState) const
{
        m_game->setNextStateToTransitionTo(newState);
        m_game->initiateFadingOut();
}

void cGameInterface::initiateFadingOut() const
{
    m_game->initiateFadingOut();
}

void cGameInterface::setNextStateToTransitionTo(int newState) const
{
    m_game->setNextStateToTransitionTo(newState);
}

void cGameInterface::loadSkirmishMaps() const
{
    m_game->loadSkirmishMaps();
}

void cGameInterface::initSkirmish() const
{
    m_game->initSkirmish();
}

void cGameInterface::loadMapFromEditor(s_PreviewMap *map) const
{
    m_game->loadMapFromEditor(map);
}

void cGameInterface::drawCursor() const
{
    m_game->getMouse()->draw();
}

cDrawManager* cGameInterface::getDrawManager() const
{
    return m_game->m_drawManager;
}

cMapCamera* cGameInterface::getMapCamera() const
{
    return m_game->m_mapCamera;
}

cRectangle* cGameInterface::getMapViewport() const
{
    return m_game->m_mapViewport;
}

void cGameInterface::setPlayerToInteractFor(cPlayer *pPlayer) const
{
    m_game->setPlayerToInteractFor(pPlayer);
}

cReinforcements* cGameInterface::getReinforcements() const
{
    return m_game->m_reinforcements.get();
}

void cGameInterface::setMissionWon() const
{
    m_game->setMissionWon();
}

void cGameInterface::setMissionLost() const
{
    m_game->setMissionLost();
}

void cGameInterface::missionInit() const
{
    m_game->missionInit();
}

void cGameInterface::setupPlayers() const
{
    m_game->setupPlayers();
}

void cGameInterface::onNotifyGameEvent(const s_GameEvent &event) const
{
    m_game->onNotifyGameEvent(event);
}

void cGameInterface::checkMissionWinOrFail() const
{
    m_game->checkMissionWinOrFail();
}

void cGameInterface::drawTextFps() const
{
    m_game->drawTextFps();
}

void cGameInterface::drawTextTime() const
{
    m_game->drawTextTime();
}

void cGameInterface::reduceShaking() const
{
    m_game->reduceShaking();
}

void cGameInterface::shakeScreen(int duration) const
{
    m_game->shakeScreen(duration);
}

void cGameInterface::goingToWinLoseBrief(int winOrLoseBrief) const
{
    m_game->goingToWinLoseBrief(winOrLoseBrief);
}

Texture* cGameInterface::getScreenTexture() const
{
    return m_game->getScreenTexture();
}

void cGameInterface::jumpToSelectYourNextConquestMission(int index) const
{
    m_game->jumpToSelectYourNextConquestMission(index);
}

void cGameInterface::loadMapFromEditor(int iSkirmishMap) const
{
    m_game->loadMapFromEditor(iSkirmishMap);
}

void cGameInterface::prepareMentatForPlayer() const
{
    m_game->prepareMentatForPlayer();
}

void cGameInterface::loadScenario() const
{
    m_game->loadScenario();
}

Color cGameInterface::getColorFadeSelectedLimited(int r, int g, int b, float minFade) const
{
    return m_game->getColorFadeSelectedLimited(r, g, b, minFade);
}

Color cGameInterface::getColorFadeSelected(int r, int g, int b) const
{
    return m_game->getColorFadeSelected(r, g, b);
}

void cGameInterface::setWinFlags(int value) const
{
    m_game->setWinFlags(value);
}

void cGameInterface::setLoseFlags(int value) const
{
    m_game->setLoseFlags(value);
}

bool cGameInterface::playMusicByType(int iType) const
{
    return m_game->playMusicByType(iType);
}

bool cGameInterface::playMusicByType(int iType, int playerId, bool triggerWithVoice) const
{
    return m_game->playMusicByType(iType, playerId, triggerWithVoice);
}

void cGameInterface::playVoice(int sampleId, int playerId) const
{
    m_game->playVoice(sampleId, playerId);
}

void cGameInterface::playSound(int sampleId) const
{
    m_game->playSound(sampleId);
}

void cGameInterface::playSoundWithDistance(int sampleId, int distance) const
{
    m_game->playSoundWithDistance(sampleId, distance);
}

int cGameInterface::getTotalPowerUsageForPlayer(cPlayer *pPlayer) const
{
    return m_game->m_structureUtils->getTotalPowerUsageForPlayer(pPlayer);
}

int cGameInterface::getTotalPowerOutForPlayer(cPlayer *pPlayer) const
{
    return m_game->m_structureUtils->getTotalPowerOutForPlayer(pPlayer);
}

int cGameInterface::getTotalSpiceCapacityForPlayer(cPlayer *pPlayer) const
{
    return m_game->m_structureUtils->getTotalSpiceCapacityForPlayer(pPlayer);
}

int cGameInterface::getStructureTypeByUnitBuildId(int unitBuildId) const
{
    return m_game->m_structureUtils->getStructureTypeByUnitBuildId(unitBuildId);
}

cDrawManager* cGameInterface::getRenderDrawManager() const
{
    return m_game->m_drawManager;
}

int cGameInterface::getCurrentState() const
{
    return m_game->getCurrentState();
}