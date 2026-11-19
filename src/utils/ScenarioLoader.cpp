/**
 * @file ScenarioLoader.cpp
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

#include "utils/ScenarioLoader.hpp"
#include "gameobjects/map/TerrainTypes.hpp"

#include <filesystem>
#include "utils/ScenarioData.hpp"
#include "iniSections.h"
#include "iniWords.h"
#include "iniUnitsParts.h"
#include "cEnumArray.h"
#include "utils/cIniUtils.h"
#include "utils/IniParsing.hpp"

#include "data/gfxdata.h"
#include "definitions.h"
#include "include/sDataCampaign.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "managers/cDrawManager.h" // TODO: an ini file reader should not depend on drawing code
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/map/cPreviewMaps.h"
#include "gameobjects/mentat/AbstractMentat.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "utils/Log.h"
#include "utils/common.h"
#include "gameobjects/map/SeedMapLoader.hpp"
#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/sTerrainInfo.h"
#include "utils/RNG.hpp"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/sGameServices.h"
#include "context/GameContext.hpp"
#include "game/MissionFlow.hpp"
#include "game/cGameConditionChecker.h"
#include "game/cGameSettings.h"
#include "gameobjects/projectiles/cBulletInfos.h"
#include <format>
#include <filesystem>
namespace fs=std::filesystem;

#include <algorithm>
#include <array>
#include <charconv>
#include <sstream>
#include <span>
#include <string>
#include <unordered_map>
#include <optional>
#include <utility>
#include <vector>
#include <platform.h>

class cReinforcements;

ScenarioLoader::ScenarioLoader(sGameServices* services)
{
    m_settings = services->settings;
    m_objects = services->objects;
    m_infos = services->info;
    m_missionFlow = services->missionFlow;
    m_eventEmitter = services->eventEmitter;
    m_services = services;
    m_gameConditionChecker = services->gameConditionChecker;
}

void ScenarioLoader::loadRegionfile(std::span<cRegion> world, int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState)
{
    auto filename = std::format("campaign/{}/mission{}.ini", getHouseDirectoryName(iHouse), iMission);
    Logger::info(COMP_REGIONINI, "ScenarioLoader::loadRegionfile", "Opening mission file: {}", filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_REGIONINI, "ScenarioLoader::loadRegionfile", "[CAMPAIGN] Error, could not open file");
        return;
    }

    auto data = parseRegionFile(file);

    for (const auto& [number, region] : data.worldRegions) {
        if (number >= static_cast<int>(world.size())) continue;
        if (region.house) world[number].iHouse = *region.house;
        if (region.selectable) world[number].bSelectable = *region.selectable;
        if (region.alpha) world[number].iAlpha = *region.alpha;
    }

    for (int i = 0; i < static_cast<int>(data.conquests.size()); i++) {
        const auto& conquest = data.conquests[i];
        selectYourNextConquestState->setRegionConquer(i, conquest.regionConquer);
        if (conquest.house) selectYourNextConquestState->setRegionHouse(i, *conquest.house);
        if (conquest.text) selectYourNextConquestState->setRegionText(i, conquest.text->c_str());
    }
    Logger::info(COMP_REGIONINI, "ScenarioLoader::loadRegionfile", "[CAMPAIGN] Done");
}

void ScenarioLoader::loadScenario(AbstractMentat *pMentat, cReinforcements *reinforcements, s_DataCampaign *dataCampaign)
{
    m_settings->setSkirmish(false);
    m_missionFlow->init();
    int iHouse = dataCampaign->housePlayer;
    int iRegion = dataCampaign->region;

    std::string filename = cIniUtils::getScenarioFileName(iHouse, iRegion);

    dataCampaign->mission = getTechLevelByRegion(iRegion);

    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load", "[SCENARIO] '{}' (Mission {})", filename, dataCampaign->mission);
    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load", "[SCENARIO] Opening file");

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error(COMP_SCENARIOINI, "ScenarioLoader::INI_Load", "[SCENARIO] Error, could not open file");
        return;
    }
    auto data = parseScenario(file);

    populateScenario(data, pMentat, reinforcements, iHouse);
}

void ScenarioLoader::populateScenario(const ScenarioData& data, AbstractMentat *pMentat, cReinforcements *reinforcements, int iHouse)
{
    if (data.briefScene) {
        pMentat->loadScene(*data.briefScene);
    }
    if (data.focusCell) {
        m_objects->getPlayer(0)->setFocusCell(*data.focusCell);
        m_services->mapCamera->centerAndJumpViewPortToCell(*data.focusCell);
    }
    if (data.winFlags) {
        m_gameConditionChecker->setWinFlags(*data.winFlags);
    }
    if (data.loseFlags) {
        m_gameConditionChecker->setLoseFlags(*data.loseFlags);
    }

    auto mapEditor = cMapEditor(m_objects->getMap());

    for (const auto& seed : data.mapLines) {
        m_objects->getMap()->init(64, 64);
        if (seed) {
            SeedMapLoader::loadIntoMap(m_objects->getMap(), m_objects->getMapGeometry(), *seed);
        }
    }

    // setupPlayers is required, because we do matching based on name of house, hence, once
    // we encounter either a UNITS or STRUCTURES entry, the assumption is made that all HOUSE information
    // has been processed.
    bool bSetUpPlayers = true;
    auto setupPlayersOnce = [&]() {
        if (!bSetUpPlayers) return;
        INI_Scenario_SetupPlayers(data.humanPlayerId, data.playerCredits, data.playerHouse, data.playerQuota);
        bSetUpPlayers = false;
    };

    auto populateReinforcement = [&](const ScenarioData::Reinforcement& reinforcement) {
        int playerId = -1;
        if (reinforcement.house > -1) {
            playerId = findPlayerIdByHouse(reinforcement.house);
        }

        int targetCell = -1;
        switch (reinforcement.location) {
            case ScenarioData::Reinforcement::Location::HOMEBASE:
                targetCell = m_objects->getPlayer(playerId)->getFocusCell();
                break;
            case ScenarioData::Reinforcement::Location::ENEMYBASE:
                if (playerId == 0) {
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (m_objects->getPlayer(i)->getHouse() == iHouse && i != playerId) {
                            targetCell = m_objects->getPlayer(i)->getFocusCell();
                            break;
                        }
                    }
                }
                else {
                    targetCell = m_objects->getPlayer(0)->getFocusCell();
                }
                break;
            case ScenarioData::Reinforcement::Location::CELL:
                targetCell = reinforcement.cell;
                break;
            case ScenarioData::Reinforcement::Location::INVALID:
                break;
        }

        bool repeat = reinforcement.repeat && m_settings->isAllowRepeatingReinforcements();
        reinforcements->addReinforcement(playerId, reinforcement.unitType, targetCell, reinforcement.delay, repeat);
    };

    for (const auto& entry : data.entries) {
        if (const auto* unit = std::get_if<ScenarioData::Unit>(&entry)) {
            setupPlayersOnce();
            int iController = -1;
            if (unit->house) {
                iController = findPlayerIdByHouse(*unit->house);
                if (iController < 0) {
                    Logger::warn(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_Section_Units", "WARNING: Cannot identify house/controller -> STRING '{}'", unit->houseName);
                }
            }
            if (iController > -1) {
                cUnits::unitCreate(m_objects, m_infos, m_eventEmitter, unit->cell, unit->type, iController, true);
            }
        }
        else if (const auto* structure = std::get_if<ScenarioData::Structure>(&entry)) {
            setupPlayersOnce();
            int iController = -1;
            if (structure->house) {
                iController = findPlayerIdByHouse(*structure->house);
                if (iController < 0 && !structure->generated) {
                    Logger::warn(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?): {}", structure->houseName);
                }
            }
            if (iController > -1) {
                m_objects->getPlayer(iController)->placeStructure(structure->cell, structure->type, 100);
            }
            else {
                Logger::warn(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_Section_Structures", "WARNING: Identifying house/controller of structure (typo?)");
            }
        }
        else if (const auto* reinforcement = std::get_if<ScenarioData::Reinforcement>(&entry)) {
            populateReinforcement(*reinforcement);
        }
    }

    mapEditor.smoothMap();

    auto toMapCell = [&](int original) {
        return m_objects->getMapGeometry()->makeCell(original % 64, original / 64);
    };

    for (int bloom : data.blooms) {
        if (bloom > -1) {
            int cell = toMapCell(bloom);
            if (cell > -1) {
                Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice BLOOM at cell : {}", cell);
                mapEditor.createCell(cell, TERRAIN_BLOOM, 0);
            }
        }
    }

    for (int field : data.fields) {
        if (field > -1) {
            int cell = toMapCell(field);
            if (cell > -1) {
                Logger::debug(COMP_SCENARIOINI, "[SCENARIO]", "Placing spice FIELD at cell : {}", cell);
                mapEditor.createRandomField(cell, TERRAIN_SPICE, 25 + (RNG::rnd(50)));
            }
        }
    }

    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load", "[SCENARIO] Done reading");

    mapEditor.smoothMap();
    m_objects->getMap()->setDesiredAmountOfWorms(m_objects->getPlayer(AI_WORM)->getAmountOfUnitsForType(SANDWORM));
}

int ScenarioLoader::findPlayerIdByHouse(int house) const
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_objects->getPlayer(i)->getHouse() == house) {
            return i;
        }
    }
    return -1;
}

void ScenarioLoader::INI_Scenario_SetupPlayers(int iHumanID, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota)
{
    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_SetupPlayers", "INI: Going to setup players");
    int iCPUId = 1; // index for CPU's, starts at 1 because ID 0 is HUMAN player

    int teamIndexAI = 1;
    bool fremenIsHumanAlly = false;

    for (int playerIndex = 0; playerIndex < MAX_PLAYERS; playerIndex++) {
        int houseForPlayer = iPl_house[playerIndex];
        Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_SetupPlayers", "House for id [{}] is [{}] - human id is [{}]", playerIndex, houseForPlayer, iHumanID);
        if (houseForPlayer > -1) {
            int creditsPlayer = iPl_credits[playerIndex];
            int quota = iPl_quota[playerIndex];

            if (playerIndex == iHumanID) {
                Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_SetupPlayers", "INI: Setting up human player, credits to [{}], house [{}] and team [{}]", creditsPlayer, houseForPlayer, 0);
                m_objects->getPlayer(HUMAN)->setCredits(creditsPlayer);
                m_objects->getPlayer(HUMAN)->setHouse(houseForPlayer);
                m_objects->getPlayer(HUMAN)->setTeam(0);
                m_objects->getPlayer(HUMAN)->setAutoSlabStructures(false);

                m_objects->getPlayer(AI_CPU5)->setHouse(FREMEN);
                if (houseForPlayer == ATREIDES) {
                    fremenIsHumanAlly = true;
                }

                d2tm_assert(m_services->drawManager);
                m_services->drawManager->missionInit();

                if (quota > 0) {
                    m_objects->getPlayer(HUMAN)->setQuota(quota);
                }
            }
            else {
                m_objects->getPlayer(iCPUId)->setAutoSlabStructures(true);

                if (quota > 0) {
                    m_objects->getPlayer(iCPUId)->setQuota(quota);
                }

                if (houseForPlayer == FREMEN) {
                    d2tm_assert(false && "No FREMEN supported in INI files yet");
                }

                m_objects->getPlayer(iCPUId)->setTeam(teamIndexAI);

                Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Scenario_SetupPlayers", "INI: Setting up CPU player, credits to [{}], house to [{}] and team [{}]", creditsPlayer, houseForPlayer, teamIndexAI);

                m_objects->getPlayer(iCPUId)->setCredits(creditsPlayer);
                m_objects->getPlayer(iCPUId)->setHouse(houseForPlayer);
                iCPUId++;
            }
        }
    }

    // CPU slots this scenario doesn't use may still hold a house from the region screen
    // (which sets house = player index); clear them so they don't count as enemies
    for (int unusedCPUId = iCPUId; unusedCPUId < AI_CPU5; unusedCPUId++) {
        m_objects->getPlayer(unusedCPUId)->setHouse(GENERALHOUSE);
    }

    if (fremenIsHumanAlly) {
        m_objects->getPlayer(AI_CPU5)->setTeam(0);
    }
    else {
        m_objects->getPlayer(AI_CPU5)->setTeam(teamIndexAI);
    }

    m_objects->getPlayer(AI_WORM)->setTeam(2); // the WORM player is nobody's ally, ever
}

void ScenarioLoader::loadBriefing(int iHouse, int iScenarioFind, eScenarioIniSection iSectionFind, AbstractMentat *pMentat)
{
    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load_Briefing", "[BRIEFING] Opening file");

    pMentat->initSentences();
    auto path = std::string("campaign/briefings/") + getBriefingFileName(iHouse);

    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load_Briefing", "{}", path);
    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load_Briefing", "Going to find SCEN ID #{} and SectionID {}", iScenarioFind, std::to_underlying(iSectionFind));

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::error(COMP_SCENARIOINI, "ScenarioLoader::INI_Load_Briefing", "[BRIEFING] Error, could not open file");
        return;
    }

    auto sentences = parseBriefing(file, iScenarioFind, iSectionFind);
    for (int i = 0; i < static_cast<int>(sentences.size()); i++) {
        pMentat->setSentence(i, sentences[i].c_str());
    }
    Logger::info(COMP_SCENARIOINI, "ScenarioLoader::INI_Load_Briefing", "[BRIEFING] File opened");
}


