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
};