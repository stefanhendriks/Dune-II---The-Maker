#ifndef D2TM_COPTIONSSTATE_H
#define D2TM_COPTIONSSTATE_H

class cGame;

#include <gui/cGuiButton.h>
#include <gui/cGuiWindow.h>
#include "cGameState.h"

class cOptionsState : public cGameState {

public:
    cOptionsState(cGame& theGame, BITMAP *background);
    ~cOptionsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

    eGameStateType getType() override;

private:
    BITMAP *background;

    const cTextDrawer textDrawer;

    cGuiWindow * gui_window;
};


#endif //D2TM_COPTIONSSTATE_H
