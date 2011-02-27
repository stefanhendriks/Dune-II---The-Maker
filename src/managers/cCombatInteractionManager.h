/*
 * cCombatInteractionManager.h
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#ifndef CCOMBATINTERACTIONMANAGER_H_
#define CCOMBATINTERACTIONMANAGER_H_

class cCombatInteractionManager : public cInteractionManager {
	public:
		cCombatInteractionManager(cPlayer * thePlayer);
		virtual ~cCombatInteractionManager();

		void interactWithMouse();
		void interactWithKeyboard();

	protected:
		void interactWithSidebar();

	private:
		cSideBar * sidebar;
};

#endif /* CCOMBATINTERACTIONMANAGER_H_ */
