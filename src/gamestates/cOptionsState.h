#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiWindow.h"
#include "sMouseEvent.h"

struct SDL_Surface;
class cGame;
class GameContext;

class cOptionsState : public cGameState {
public:
    explicit cOptionsState(cGame &theGame, GameContext *context, int prevState);
    ~cOptionsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

    void setPrevState(int prevState);
    void refresh();

private:
    cTextDrawer m_textDrawer;

    int m_prevState;

    GuiWindow *m_guiWindow;

    void constructWindow(int prevState);
    GameContext *m_context;
};
