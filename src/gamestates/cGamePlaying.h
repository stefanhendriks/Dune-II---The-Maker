#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

class Texture;
class cGame;
class cReinforcements;



class cGamePlaying : public cGameState {
public:
    explicit cGamePlaying(cGame &theGame, GameContext* ctx);
    ~cGamePlaying() override;

    void thinkFast() override;
    void thinkNormal() override;
    void thinkSlow() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

    void missionInit();
private:
    void evaluatePlayerStatus();
    void drawCombatMouse() const;

    int m_TIMER_evaluatePlayerStatus;
    cReinforcements* m_reinforcements;

    // win/lose flags
    int8_t m_winFlags, m_loseFlags;

    void onKeyDownGamePlaying(const cKeyboardEvent &event);
    void onKeyPressedGamePlaying(const cKeyboardEvent &event);
    void onKeyDownDebugMode(const cKeyboardEvent &event);
};
