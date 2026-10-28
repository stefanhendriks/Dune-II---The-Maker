/**
 * @file ini.h
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

#pragma once

#include <span>
#include <string>
#include "iniSections.h"

class cRegion;
class cReinforcements;
class AbstractMentat;
class cSelectYourNextConquestState;
struct s_DataCampaign;
struct ScenarioData;

class cGameSettings;
class cGameObjectContext;
class cInfoContext;
class cGameInterface;
class cEventEmitter;
class cGameConditionChecker;
struct sGameServices;

// public stuff
class cIni {
public:
    explicit cIni(sGameServices* services);
    // Read game.ini file
    void installGame(const std::string& filename);
    // Load original scenario ini file
    void loadScenario(AbstractMentat *pMentat, cReinforcements *reinforcements, s_DataCampaign *dataCampaign);
    // Load briefing for original scenario
    void loadBriefing(int iHouse, int iScenarioFind, eScenarioIniSection iSectionFind, AbstractMentat *pMentat);
    // Load regionfile for original scenario
    void loadRegionfile(std::span<cRegion> world, int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);


private:
    void populateScenario(const ScenarioData& data, AbstractMentat *pMentat, cReinforcements *reinforcements, int iHouse);
    int  findPlayerIdByHouse(int house) const;
    void INI_Scenario_SetupPlayers(int iHumanID, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota);

    cGameSettings* m_settings = nullptr;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cGameInterface* m_interface = nullptr;
    cEventEmitter* m_eventEmitter = nullptr;
    sGameServices* m_services = nullptr;
    cGameConditionChecker* m_gameConditionChecker = nullptr;
};
