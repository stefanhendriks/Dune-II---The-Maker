/*
 * cInteractionManager.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 *
 *  The interaction manager, manages the interaction of mouse/keyboard of certain elements.
 *  Ie, the sidebar can be interacted with by Mouse/Keyboard.
 *
 */

#ifndef CINTERACTIONMANAGER_H_
#define CINTERACTIONMANAGER_H_

class cPlayer;
class cSideBar;
class cMiniMapDrawer;
class cMouseDrawer;
class cPlaceItDrawer;
class cOrderDrawer;

#include "cKeyboardManager.h"

class cInteractionManager {
	public:
		cInteractionManager(cPlayer * thePlayer);
		~cInteractionManager();

		void onMouseAt(int x, int y);
		void onMouseClickedLeft(int x, int y);
		void onMouseClickedRight(int x, int y);
		void onMouseScrolledUp();
		void onMouseScrolledDown();

		void interactWithKeyboard();

		void setPlayerToInteractFor(cPlayer * thePlayer);

	private:
		cSideBar * sidebar;
		cMiniMapDrawer * miniMapDrawer;
		cKeyboardManager keyboardManager;
		cPlayer * m_Player;
		cMouseDrawer * mouseDrawer;
		cPlaceItDrawer * placeItDrawer;
		cOrderDrawer * orderDrawer;

};

#endif /* CINTERACTIONMANAGER_H_ */
