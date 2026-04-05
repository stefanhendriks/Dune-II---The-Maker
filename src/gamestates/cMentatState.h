#pragma once

#include "cGameState.h"
#include "mentat/AbstractMentat.h"

struct s_DataCampaign;
class cReinforcements;

enum class eMentatMode {
    Briefing,
    WinBrief,
    LoseBrief
};

class cMentatState : public cGameState {
public:
    cMentatState(cGame &game, GameContext* ctx, eMentatMode mode, s_DataCampaign* dataCampaign);
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
    AbstractMentat* m_mentat = nullptr;
    s_DataCampaign* m_dataCampaign = nullptr;
    eMentatMode m_mode;
    int m_house;
};