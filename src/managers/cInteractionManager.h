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

	protected:
		void interactWithSidebar();

	private:
		cSideBar * sidebar;
		cKeyboardManager keyboardManager;
};

#endif /* CINTERACTIONMANAGER_H_ */
