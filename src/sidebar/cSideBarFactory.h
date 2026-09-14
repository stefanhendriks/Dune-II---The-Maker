/**
 * @file cSideBarFactory.h
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
