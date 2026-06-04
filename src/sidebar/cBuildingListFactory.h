#pragma once

#include "sidebar/cBuildingList.h"

class cGameSettings;

class cBuildingListFactory {

public:
    explicit cBuildingListFactory(cGameSettings* settings);
    ~cBuildingListFactory() = default;
    void initializeList(cBuildingList *list, eListType listType);
    cBuildingList *createList(eListType listType);

protected:
    int getButtonDrawY();
    int getButtonDrawXStart();

private:
    cGameSettings* m_settings;
};
