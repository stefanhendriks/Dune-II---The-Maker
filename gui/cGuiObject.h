#ifndef D2TM_CGUIOBJECT_H
#define D2TM_CGUIOBJECT_H

#include "utils/cRectangle.h"
#include "../observers/cMouseObserver.h"
#include "cGuiAction.h"

class cGuiObject : cMouseObserver {
public:
    virtual ~cGuiObject() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;

    // cGuiObject specific
    virtual void draw() const = 0; // pure virtual function

protected:
    // any gui object has a position and size. Hence its always a 'rect'.
    cRectangle rect;

private:

};


#endif //D2TM_CGUIOBJECT_H
