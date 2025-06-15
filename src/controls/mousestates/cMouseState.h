#pragma once

#include "observers/cInputObserver.h"
#include "observers/cScenarioObserver.h"
#include "controls/cMouse.h"

class cGameControlsContext;
class cPlayer;

class cMouseState : public cInputObserver, cScenarioObserver {

public:
    cMouseState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override = 0;
    void onNotifyGameEvent(const s_GameEvent &event) override = 0;

    virtual void onStateSet() = 0; // called when switched to this mouse state
    virtual void onFocus() = 0; // called when state regains focus
    virtual void onBlur() = 0; // called when state loses focus

protected:
    cPlayer * m_player;
    cGameControlsContext * m_context;
    cMouse * m_mouse;

    int mouseTile;

};