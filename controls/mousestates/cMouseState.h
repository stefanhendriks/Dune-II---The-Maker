#ifndef D2TM_CMOUSESTATE_H
#define D2TM_CMOUSESTATE_H

#include "observers/cMouseObserver.h"
#include "controls/cMouse.h"
//#include "controls/cGameControlsContext.h"

// forward declaration
class cGameControlsContext;

class cMouseState : public cMouseObserver {

public:
    cMouseState(cGameControlsContext *context, cMouse * mouse);
    ~cMouseState();

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;

protected:
    cGameControlsContext * context;
    cMouse * mouse;
    int mouseTile;

};


#endif //D2TM_CMOUSESTATE_H
