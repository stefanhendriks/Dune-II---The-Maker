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

class cInteractionManager {
	public:
		cInteractionManager(cPlayer * thePlayer);
		~cInteractionManager();

		void interact();
		void interactWithKeyboard();

		void setPlayerToInteractFor(cPlayer * thePlayer);

	protected:
		void mouseInteractWithSidebarOrMinimap();

	private:
		cSideBar * sidebar;
		cMiniMapDrawer * miniMapDrawer;
		cKeyboardManager keyboardManager;
		cPlayer * m_Player;


    void mouseInteractWithBattlefield();
};

#endif /* CINTERACTIONMANAGER_H_ */
