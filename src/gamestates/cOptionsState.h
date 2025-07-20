#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/cGuiWindow.h"
#include "sMouseEvent.h"

struct SDL_Surface;
class cGame;

class cOptionsState : public cGameState {
public:
    explicit cOptionsState(cGame &theGame, /*SDL_Surface *background,*/ int prevState);
    ~cOptionsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

    void setPrevState(int prevState);
    void refresh();

private:
    // SDL_Surface *m_background;

    cTextDrawer m_textDrawer;

    int m_prevState;

    GuiWindow *m_guiWindow;

    void constructWindow(int prevState);
};
