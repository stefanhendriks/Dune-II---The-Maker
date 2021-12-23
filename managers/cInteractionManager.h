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
struct s_MouseEvent;

#include "cKeyboardManager.h"
#include "../observers/cInputObserver.h"

class cInteractionManager : public cInputObserver {
	public:
		cInteractionManager(cPlayer * thePlayer);
		~cInteractionManager();

		void onNotifyMouseEvent(const s_MouseEvent &mouseEvent) override;
        void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;

        void interactWithKeyboard();

		void setPlayerToInteractFor(cPlayer * thePlayer);

	private:
		cSideBar * sidebar;
		cMiniMapDrawer * miniMapDrawer;
		cKeyboardManager keyboardManager;
		cPlayer * player;
		cMouseDrawer * mouseDrawer;
		cPlaceItDrawer * placeItDrawer;
		cOrderDrawer * orderDrawer;

        void onMouseAt(const s_MouseEvent &mouseEvent);
        void onMouseClickedLeft(const s_MouseEvent &mouseEvent);
        void onMouseClickedRight(const s_MouseEvent &mouseEvent);
        void onMouseScrolledUp(const s_MouseEvent &mouseEvent);
        void onMouseScrolledDown(const s_MouseEvent &mouseEvent);
};

#endif /* CINTERACTIONMANAGER_H_ */
