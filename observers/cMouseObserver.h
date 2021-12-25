#ifndef D2TM_CMOUSEOBSERVER_H
#define D2TM_CMOUSEOBSERVER_H

#include "../include/sMouseEvent.h"

class cMouseObserver {

public:
    virtual ~cMouseObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) = 0;
};


#endif //D2TM_CMOUSEOBSERVER_H
