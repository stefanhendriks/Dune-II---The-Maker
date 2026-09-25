/**
 * @file cBuildingListFactory.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
