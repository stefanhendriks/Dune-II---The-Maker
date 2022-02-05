#pragma once

class cGame;

#include "gui/cGuiButton.h"
#include "gui/cGuiWindow.h"
#include "cGameState.h"

class cOptionsState : public cGameState {
  public:
    cOptionsState(cGame& theGame, BITMAP *background, int prevState);
    ~cOptionsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

  private:
    BITMAP *background;

    int prevState;

    const cTextDrawer textDrawer;

    cGuiWindow * gui_window;
};
