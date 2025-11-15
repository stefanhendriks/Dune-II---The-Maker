#pragma once

#include "cGameState.h"
#include "mentat/AbstractMentat.h"


class cTellHouseState : public cGameState {
public:
    cTellHouseState(cGame &game, GameContext* ctx, int house = -1);
    ~cTellHouseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
    void prepareMentat(int house);

private:
    AbstractMentat* m_mentat = nullptr;
    int m_house = -1;
};