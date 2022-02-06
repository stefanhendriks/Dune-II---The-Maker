/*
 *  The interaction manager, manages the interaction of mouse/keyboard of certain elements.
 *  Ie, the sidebar can be interacted with by Mouse/Keyboard.
 *
 */

#pragma once

#include "controls/cKeyboardEvent.h"
#include "observers/cInputObserver.h"
#include "observers/cScenarioObserver.h"
#include "sGameEvent.h"
#include "sMouseEvent.h"

class cMiniMapDrawer;
class cMouseDrawer;
class cPlaceItDrawer;
class cPlayer;
class cOrderDrawer;
class cSideBar;

class cInteractionManager : public cInputObserver, cScenarioObserver {
	public:
		explicit cInteractionManager(cPlayer * thePlayer);
		~cInteractionManager();

		void onNotifyMouseEvent(const s_MouseEvent &mouseEvent) override;
        void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
        void onNotifyGameEvent(const s_GameEvent &) override {}

		void setPlayerToInteractFor(cPlayer * thePlayer);

	private:
		cSideBar * sidebar;
		cMiniMapDrawer * miniMapDrawer;
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
