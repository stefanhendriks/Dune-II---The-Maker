#pragma once

#include "cGameState.h"
#include "mentat/AbstractMentat.h"

#include <memory>

struct s_DataCampaign;

class cTellHouseState : public cGameState {
public:
    cTellHouseState(cGame &game, sGameServices* services, s_DataCampaign* dataCampaign);
    ~cTellHouseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
    void prepareMentat(int house);

private:
    std::unique_ptr<AbstractMentat> m_mentat;
    int m_house = -1;
    s_DataCampaign* m_dataCampaign;
};