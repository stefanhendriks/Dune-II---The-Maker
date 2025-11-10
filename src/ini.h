/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include <string>

class cReinforcements;
class AbstractMentat;
class cSelectYourNextConquestState;

// public stuff
class cIni {
public:
    // Read game.ini file
    static void installGame(std::string filename);
    // Load original scenario ini file
    static void loadScenario(int iHouse, int iRegion, AbstractMentat *pMentat,cReinforcements *reinforcements);
    // Load briefing for original scenario
    static void loadBriefing(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);
    // Load regionfile for original scenario
    static void loadRegionfile(int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);


private:
    // Section Units, old Code style
    static bool INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                     const int *iPl_quota, const std::string& linefeed);
    // Section Structures, old Code style
    static bool INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                          const int *iPl_quota, const std::string& linefeed);

    // Load seed for random generator to original scenario maps
    static void INI_Load_seed(int seed);


    // Reinforcements, old Code style
    static void INI_Scenario_Section_Reinforcements(int iHouse, const std::string& linefeed, cReinforcements *reinforcements);
    // Map, old Code style
    static void INI_Scenario_Section_MAP(int *blooms, int *fields, int wordtype, const std::string& linefeed);
    // Section House, old Code style
    static int INI_Scenario_Section_House(int wordtype, int iPlayerID, int *iPl_credits, int *iPl_quota, const std::string& linefeed);
    // Section_Basic, old Code style
    static void INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, const std::string& linefeed);
    // SetupPlayers, old Code style
    static void INI_Scenario_SetupPlayers(int iHumanID, const int *iPl_credits, const int *iPl_house, const int *iPl_quota);


};


