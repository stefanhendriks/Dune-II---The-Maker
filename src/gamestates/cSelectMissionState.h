#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiWindow.h"
#include "sMouseEvent.h"

struct SDL_Surface;
class cGame;

class cSelectMissionState : public cGameState {
public:
    cSelectMissionState(cGame &theGame, /*SDL_Surface *background,*/ int prevState);
    ~cSelectMissionState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    int prevState;

    cTextDrawer textDrawer;

    GuiWindow *gui_window;
};
