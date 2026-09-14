/**
 * @file cMentatState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cGameState.h"
#include "gameobjects/mentat/AbstractMentat.h"
#include <memory>

struct s_DataCampaign;
class cReinforcements;
class cGameInterface;
class cGameSettings;
class cIni;

enum class MentatMode {
    Briefing,
    WinBrief,
    LoseBrief
};

class cMentatState : public cGameState {
public:
    cMentatState(sGameServices* services, MentatMode mode, cIni* cini, s_DataCampaign* dataCampaign);
    ~cMentatState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
    void loadScenario(cReinforcements *reinforcements);
    void loadBriefing(int iScenarioFind, int iSectionFind);
    void prepareMentat(int house);
private:
    std::unique_ptr<AbstractMentat> m_mentat;

    s_DataCampaign* m_dataCampaign = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    cGameObjectContext* m_objects = nullptr;
    cIni* m_cIni = nullptr;
    MentatMode m_mode;
    int m_house;
    bool m_allowMissionSelect;
};