/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cPlayer.h"
#include "cPlayers.h"
#include "building/cItemBuilder.h"
#include "controls/cGameControlsContext.h"
#include "controls/cMouse.h"
#include "gameobjects/structures/cOrderProcesser.h"
#include "sidebar/cBuildingListUpdater.h"
#include "sidebar/cSideBarFactory.h"
#include "utils/cLog.h"
#include "brains/cPlayerBrainSkirmish.h"
#include "brains/cPlayerBrainCampaign.h"
#include "brains/superweapon/cPlayerBrainFremenSuperWeapon.h"
#include "brains/cPlayerBrainSandworm.h"
#include "game/cGameSettings.h"
#include "include/sDataCampaign.h"
#include "definitions.h"

#include "cHousesInfo.h"
#include "include/sGameEvent.h"

cPlayers::cPlayers() {
    for (int i = 0; i < MAX_PLAYERS_CAPACITY; i++) {
        m_players[i] = new cPlayer();
    }
}

cPlayers::~cPlayers() {
    for (int i = 0; i < MAX_PLAYERS_CAPACITY; i++) {
        delete m_players[i];
        m_players[i] = nullptr;
    }
}

cPlayer* cPlayers::operator[](int index) {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::operator[] out of bounds");
    return m_players[index];
}

const cPlayer* cPlayers::operator[](int index) const {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::operator[] const out of bounds");
    return m_players[index];
}

cPlayer* cPlayers::getPlayer(int index) {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::getPlayer out of bounds");
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        return nullptr;
    }
    return m_players[index];
}

const cPlayer* cPlayers::getPlayer(int index) const {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::getPlayer const out of bounds");
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        return nullptr;
    }
    return m_players[index];
}

cPlayer* cPlayers::getHumanPlayer() {
    return m_players[0];  // HUMAN is typically player 0
}

const cPlayer* cPlayers::getHumanPlayer() const {
    return m_players[0];  // HUMAN is typically player 0
}

void cPlayers::setupPlayers(cHousesInfo* housesInfo)
{
    assert(housesInfo != nullptr);
    for (int i = 0; i < MAX_PLAYERS_CAPACITY; i++) {
        m_players[i]->init(i, nullptr);
        m_players[i]->setHousesInfo(housesInfo);
    }
}

void cPlayers::setupRuntimePlayerComponents(cSideBarFactory* sideBarFactory, cMouse* mouse, int techLevel)
{
    assert(sideBarFactory != nullptr);
    assert(mouse != nullptr);

    for (int i = HUMAN; i < MAX_PLAYERS_CAPACITY; i++) {
        cPlayer* player = m_players[i];

       auto buildingListUpdater = std::make_unique<cBuildingListUpdater>(player);
        auto itemBuilder = std::make_unique<cItemBuilder>(player, buildingListUpdater.get());
        player->setBuildingListUpdater(std::move(buildingListUpdater));
        player->setItemBuilder(std::move(itemBuilder));

        auto sidebar = sideBarFactory->createSideBar(player);
        player->setSideBar(sidebar);

        auto orderProcesser = std::make_unique<cOrderProcesser>(player);
        player->setOrderProcesser(std::move(orderProcesser));

        auto gameControlsContext = std::make_unique<cGameControlsContext>(player, mouse);
        player->setGameControlsContext(std::move(gameControlsContext));

        player->setTechLevel(techLevel);
    }
}

void cPlayers::onNotifyGameEvent(const s_GameEvent& event)
{
    for (auto& player : m_players) {
        player->onNotifyGameEvent(event);
    }
}

void cPlayers::evaluateStillAliveForAI()
{
    for (int i = 1; i < MAX_PLAYERS_CAPACITY; i++) {
        m_players[i]->evaluateStillAlive();
    }
}

void cPlayers::destroyAllegroBitmaps()
{
    for (auto& player : m_players) {
        player->destroyAllegroBitmaps();
    }
}

void cPlayers::initPlayers(bool rememberHouse, cGameSettings* gameSettings, s_DataCampaign* dataCampaign)
{
    int maxThinkingAIs = MAX_PLAYERS_CAPACITY;
    if (gameSettings->isOneAi()) {
        maxThinkingAIs = 1;
    }

    for (int i = 0; i < MAX_PLAYERS_CAPACITY; i++) {
        cPlayer* pPlayer = m_players[i];

        int h = pPlayer->getHouse();

        std::unique_ptr<brains::cPlayerBrain> brain;
        bool autoSlabStructures = false;

        if (i > HUMAN && i < AI_CPU5) {
            // TODO: playing attribute? (from ai player class?)
            if (!gameSettings->isDisableAI()) {
                if (maxThinkingAIs > 0) {
                    if (gameSettings->isSkirmish()) {
                        brain = std::make_unique<brains::cPlayerBrainSkirmish>(pPlayer);
                    }
                    else {
                        brain = std::make_unique<brains::cPlayerBrainCampaign>(pPlayer, dataCampaign);
                        autoSlabStructures = true;  // campaign based AI's autoslab structures...
                    }
                }
            }
            maxThinkingAIs--;
        }
        else if (i == AI_CPU5) {
            brain = std::make_unique<brains::cPlayerBrainFremenSuperWeapon>(pPlayer);
        }
        else if (i == AI_CPU6) {
            if (!gameSettings->isDisableWormAi()) {
                brain = std::make_unique<brains::cPlayerBrainSandworm>(pPlayer);
            }
        }

        pPlayer->init(i, std::move(brain));
        pPlayer->setAutoSlabStructures(autoSlabStructures);
        if (rememberHouse) {
            pPlayer->setHouse(h);
        }

        if (gameSettings->isSkirmish()) {
            pPlayer->setCredits(2500);
        }
    }
}
