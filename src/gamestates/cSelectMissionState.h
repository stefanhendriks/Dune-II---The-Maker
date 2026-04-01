#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"

struct SDL_Surface;
class cGame;
class GameContext;
class cTextDrawer;
class GuiWindow;

class cSelectMissionState : public cGameState {
public:
    cSelectMissionState(cGame &theGame, GameContext* ctx, int prevState);
    ~cSelectMissionState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer *m_textDrawer = nullptr;

    GuiWindow *gui_window;
};
