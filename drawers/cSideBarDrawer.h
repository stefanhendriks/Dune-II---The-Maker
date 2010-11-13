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

		cBuildingListDrawer * getBuildingListDrawer() { return buildingListDrawer; }

		bool isMouseOverScrollUp();
		bool isMouseOverScrollDown();

	protected:
		void drawHouseGui(cPlayer * thePlayer);
		void drawBuildingLists();
		void drawCapacities();
		void drawScrollButtons();

		// get
		int getScrollButtonUpX();
		int getScrollButtonY();
		int getScrollButtonDownX();
		bool isOverScrollButton(int buttonX, int buttonY);

	private:
		cBuildingListDrawer * buildingListDrawer;

};

#endif /* CSIDEBARDRAWER_H_ */
