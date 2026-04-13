#pragma once

#include "sidebar/cBuildingList.h"

class cBuildingListFactory {

public:
    cBuildingListFactory();
    ~cBuildingListFactory() = default;
    void initializeList(cBuildingList *list, eListType listType);
    cBuildingList *createList(eListType listType);

protected:
    int getButtonDrawY();
    int getButtonDrawXStart();
};
