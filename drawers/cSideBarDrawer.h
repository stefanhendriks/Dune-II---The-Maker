/*
 * cSideBarDrawer.h
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#ifndef CSIDEBARDRAWER_H_
#define CSIDEBARDRAWER_H_

class cSideBarDrawer {
	public:
		cSideBarDrawer();
		virtual ~cSideBarDrawer();

		void drawSideBar(cPlayer * player);

	protected:
		void drawHouseGui(cPlayer * thePlayer);
		void drawBuildingLists();
		void drawCapacities();

	private:
		cBuildingListDrawer * buildingListDrawer;

};

#endif /* CSIDEBARDRAWER_H_ */
