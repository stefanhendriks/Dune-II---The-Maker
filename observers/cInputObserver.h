#ifndef D2TM_CINPUTOBSERVER_H
#define D2TM_CINPUTOBSERVER_H

#include "cMouseObserver.h"
#include "../include/sKeyboardEvent.h"

class cInputObserver : public cMouseObserver {

public:
    virtual ~cInputObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) override = 0;
    virtual void onNotifyKeyboardEvent(const s_KeyboardEvent &event) = 0;
};


#endif //D2TM_CINPUTOBSERVER_H
