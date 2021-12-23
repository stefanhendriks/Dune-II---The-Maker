#ifndef D2TM_CGUIOBJECT_H
#define D2TM_CGUIOBJECT_H

#include "utils/cRectangle.h"
#include "../observers/cInputObserver.h"
#include "cGuiAction.h"

class cGuiObject : cInputObserver {
public:
    virtual ~cGuiObject() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    virtual void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override = 0;

    // cGuiObject specific
    virtual void draw() const = 0; // pure virtual function

    cRectangle &getRect() { return rect; };
    void setRect(const cRectangle &newRect) { rect = newRect; }

protected:
    // any gui object has a position and size. Hence its always a 'rect'.
    cRectangle rect;

private:

};


#endif //D2TM_CGUIOBJECT_H
