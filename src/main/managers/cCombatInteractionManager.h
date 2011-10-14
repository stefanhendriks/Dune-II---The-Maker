#ifndef CCOMBATINTERACTIONMANAGER_H_
#define CCOMBATINTERACTIONMANAGER_H_

#include "../player/cPlayer.h"
#include "cInteractionManager.h"

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
