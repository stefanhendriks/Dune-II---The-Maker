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

#include <string>
#include <span>

class cRegion;
class cReinforcements;
class AbstractMentat;
class cSelectYourNextConquestState;
struct s_DataCampaign;

class cGameSettings;
class cGameObjectContext;
class cInfoContext;
class cGameInterface;
struct sGameServices;

// public stuff
class cIni {
public:
    explicit cIni(sGameServices* services);
    // Read game.ini file
    void installGame(std::string filename);
    // Load original scenario ini file
    void loadScenario(/*int iHouse, int iRegion,*/ AbstractMentat *pMentat,cReinforcements *reinforcements, s_DataCampaign *dataCampaign);
    // Load briefing for original scenario
    void loadBriefing(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);
    // Load regionfile for original scenario
    void loadRegionfile(std::span<cRegion> world,int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);


private:
    // Section Units, old Code style
    bool INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                     const int *iPl_quota, const std::string& linefeed);
    // Section Structures, old Code style
    bool INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                          const int *iPl_quota, const std::string& linefeed);

    // Load seed for random generator to original scenario maps
    void INI_Load_seed(int seed);

    // Reinforcements, old Code style
    void INI_Scenario_Section_Reinforcements(int iHouse, const std::string& linefeed, cReinforcements *reinforcements);
    // Map, old Code style
    void INI_Scenario_Section_MAP(int *blooms, int *fields, int wordtype, const std::string& linefeed);
    // Section House, old Code style
    int INI_Scenario_Section_House(int wordtype, int iPlayerID, int *iPl_credits, int *iPl_quota, const std::string& linefeed);
    // Section_Basic, old Code style
    void INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, const std::string& linefeed);
    // SetupPlayers, old Code style
    void INI_Scenario_SetupPlayers(int iHumanID, const int *iPl_credits, const int *iPl_house, const int *iPl_quota);

    cGameSettings* m_settings = nullptr;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cGameInterface* m_interface = nullptr;
};


