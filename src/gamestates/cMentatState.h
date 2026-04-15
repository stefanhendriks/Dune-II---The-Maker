#pragma once

#include "cGameState.h"
#include "mentat/AbstractMentat.h"
#include <memory>

struct s_DataCampaign;
class cReinforcements;

enum class MentatMode {
    Briefing,
    WinBrief,
    LoseBrief
};

class cMentatState : public cGameState {
public:
    cMentatState(cGame &game, sGameServices* services, MentatMode mode, s_DataCampaign* dataCampaign);
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
    MentatMode m_mode;
    int m_house;
};