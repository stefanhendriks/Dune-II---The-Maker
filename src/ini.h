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
// Read game.ini file
void INI_Install_Game(std::string filename);
// Load original scenario ini file
void INI_Load_scenario(int iHouse, int iRegion, AbstractMentat *pMentat,cReinforcements *reinforcements);
// Load briefing for original scenario
void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);

// Load regionfile for original scenario
void INI_Load_Regionfile(int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);

// private stuff
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




