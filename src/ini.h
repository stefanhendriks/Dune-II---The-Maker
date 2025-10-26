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

// Scenario loading
//void INI_Load_scenario(bool bOrDune, char filename[30] );
void INI_Load_scenario(int iHouse, int iRegion, AbstractMentat *pMentat,cReinforcements *reinforcements);
void INI_Load_seed(int seed);

void INI_Install_Game(std::string filename);
void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);
void INI_Load_Regionfile(int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);
