/**
 * @file cGameInterface.cpp
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

#include "game/cGameInterface.h"
#include "context/cGameObjectContext.h"
#include "game/cGame.h"
#include "include/Texture.hpp"
#include "include/sGameEvent.h"

#include <format>

cGameInterface::cGameInterface(cGame* game)
    : m_game(game)
{
}

void cGameInterface::prepareMentatToTellAboutHouse(int house) const
{
    m_game->prepareMentatToTellAboutHouse(house);
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

// void cGameInterface::loadSkirmishMaps() const
// {
//     m_game->loadSkirmishMaps();
// }

void cGameInterface::initSkirmish() const
{
    m_game->initSkirmish();
}

// void cGameInterface::loadMapFromEditor(s_PreviewMap *map) const
// {
//     m_game->loadMapFromEditor(map);
// }

void cGameInterface::setPlayerToInteractFor(cPlayer *pPlayer) const
{
    m_game->setPlayerToInteractFor(pPlayer);
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
    m_game->emitGameEvent(event);
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

int cGameInterface::getCurrentState() const
{
    return m_game->getCurrentState();
}

void cGameInterface::changeStateFromMentat() const
{
    m_game->changeStateFromMentat();
}

cPlayer* cGameInterface::getPlayer(int id) const
{
    return m_game->m_gameObjectsContext->getPlayer(id);
}

cGameSettings* cGameInterface::getGameSettings() const
{
    return m_game->m_gameSettings.get();
}

