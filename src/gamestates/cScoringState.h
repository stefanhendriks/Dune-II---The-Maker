#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"
#include "game/MissionStats.h"

class cGameSettings;
class MissionFlow;
class GameStateTransition;
class cTextDrawer;
class cMouse;
class cScreenFader;

class cScoringState : public cGameState {

public:
    explicit cScoringState(sGameServices* services);
    ~cScoringState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    void continueToWinBrief() const;

    cGameSettings* m_settings = nullptr;
    MissionFlow* m_missionFlow = nullptr;
    GameStateTransition* m_stateTransitions = nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    cTextDrawer* m_smallTextDrawer = nullptr;
    cMouse* m_mouse = nullptr;
    cScreenFader* m_screenFader = nullptr;
    MissionStats m_stats;
};
