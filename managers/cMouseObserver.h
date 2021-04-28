//
// Created by shendriks on 4/28/2021.
//

#ifndef D2TM_CMOUSEOBSERVER_H
#define D2TM_CMOUSEOBSERVER_H

#include "../include/sMouseEvent.h"

class cMouseObserver {
public:
    virtual ~cMouseObserver();

    virtual void onNotify(const s_MouseEvent &event) = 0;
};


#endif //D2TM_CMOUSEOBSERVER_H
