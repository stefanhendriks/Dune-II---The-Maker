#pragma once

#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"

class cInputObserver {
public:
    virtual ~cInputObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) = 0;
    virtual void onNotifyKeyboardEvent(const cKeyboardEvent &event) = 0;
};
