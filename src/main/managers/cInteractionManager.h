/*
 * cInteractionManager.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 *
 *  The interaction manager, manages the interaction of mouse/keyboard of certain elements.
 *  Ie, the sidebar can be interacted with by Mouse/Keyboard. This is the interface, several
 *  states need different interaction managers.
 *
 */

#ifndef CINTERACTIONMANAGER_H_
#define CINTERACTIONMANAGER_H_

#include "cKeyboardManager.h"
#include "../player/cPlayer.h"

class cInteractionManager {
	public:
		cInteractionManager(cPlayer * thePlayer);
		virtual ~cInteractionManager();

		virtual void interactWithMouse() = 0;
		void interactWithKeyboard();

	protected:
		cPlayer * player;

	private:
		cKeyboardManager keyboardManager;

};

#endif /* CINTERACTIONMANAGER_H_ */
