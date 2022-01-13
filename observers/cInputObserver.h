#ifndef D2TM_CINPUTOBSERVER_H
#define D2TM_CINPUTOBSERVER_H

#include "controls/cKeyboardEvent.h"
#include "include/sGameEvent.h"
#include "include/sMouseEvent.h"

class cInputObserver {

public:
    virtual ~cInputObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) = 0;
    virtual void onNotifyKeyboardEvent(const cKeyboardEvent &event) = 0;
};


#endif //D2TM_CINPUTOBSERVER_H
