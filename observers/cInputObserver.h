#ifndef D2TM_CINPUTOBSERVER_H
#define D2TM_CINPUTOBSERVER_H

#include "../include/sMouseEvent.h"
#include "../include/sKeyboardEvent.h"

class cInputObserver {

public:
    virtual ~cInputObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) = 0;
    virtual void onNotifyKeyboardEvent(const s_KeyboardEvent &event) = 0;
};


#endif //D2TM_CINPUTOBSERVER_H
