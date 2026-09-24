#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"
#include "game/MissionStats.h"
#include "gui/GuiButton.h"

#include <memory>

class cGameSettings;
class cGameInterface;
class cTextDrawer;

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
    cGameInterface* m_interface = nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    MissionStats m_stats;
    std::unique_ptr<GuiButton> m_continueButton;
};
