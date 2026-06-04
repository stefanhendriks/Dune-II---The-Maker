#pragma once

class cSideBar;
class cPlayer;
class cBuildingListFactory;
struct sGameServices;

class cSideBarFactory {
public:
    cSideBarFactory(cBuildingListFactory* buildingListFactory, sGameServices* services);
    ~cSideBarFactory();
    cSideBar *createSideBar(cPlayer *thePlayer);
private:
    cBuildingListFactory* m_buildingListFactory;
    sGameServices* m_services;
};
