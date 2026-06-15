/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include <span>
#include <string>

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
    void installGame(const std::string& filename);
    // Load original scenario ini file
    void loadScenario(AbstractMentat *pMentat, cReinforcements *reinforcements, s_DataCampaign *dataCampaign);
    // Load briefing for original scenario
    void loadBriefing(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat);
    // Load regionfile for original scenario
    void loadRegionfile(std::span<cRegion> world, int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState);


private:
    void INI_Load_seed(int seed);
    void INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, const std::string& value);
    int  INI_Scenario_Section_House(int wordtype, int iPlayerID, std::span<int> iPl_credits, std::span<int> iPl_quota, const std::string& value);
    void INI_Scenario_Section_MAP(std::span<int> blooms, std::span<int> fields, int wordtype, const std::string& linefeed);
    void INI_Scenario_Section_Reinforcements(int iHouse, const std::string& linefeed, cReinforcements *reinforcements);
    bool INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota, const std::string& linefeed);
    bool INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota, const std::string& linefeed);
    void INI_Scenario_SetupPlayers(int iHumanID, std::span<const int> iPl_credits, std::span<const int> iPl_house, std::span<const int> iPl_quota);

    cGameSettings* m_settings = nullptr;
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cGameInterface* m_interface = nullptr;
};
