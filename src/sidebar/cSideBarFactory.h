#pragma once

#include "cSideBar.h"
#include "player/cPlayer.h"

class cSideBarFactory {

private:
    static cSideBarFactory *instance;

protected:
    cSideBarFactory();
    ~cSideBarFactory();

public:
    static cSideBarFactory *getInstance();
    static void destroy();

    cSideBar *createSideBar(cPlayer *thePlayer);

private:

};
