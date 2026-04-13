#pragma once

class cSideBar;
class cPlayer;

class cSideBarFactory {
public:
    cSideBar *createSideBar(cPlayer *thePlayer);
    cSideBarFactory();
    ~cSideBarFactory();
private:

};
