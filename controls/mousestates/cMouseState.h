#ifndef D2TM_CMOUSESTATE_H
#define D2TM_CMOUSESTATE_H

#include "observers/cInputObserver.h"
#include "controls/cMouse.h"

class cGameControlsContext;
class cPlayer;

class cMouseState : public cInputObserver {

public:
    cMouseState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    virtual void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override = 0;
    virtual void onStateSet() = 0; // called when switched to this mouse state

protected:
    cPlayer * player;
    cGameControlsContext * context;
    cMouse * mouse;

    int mouseTile;

};


#endif //D2TM_CMOUSESTATE_H
