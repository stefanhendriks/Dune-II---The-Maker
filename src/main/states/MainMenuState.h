#ifndef MAIN_MENU_STATE_H_
#define MAIN_MENU_STATE_H_

#include <assert.h>
#include <cstddef>

#include "State.h"

#include "../managers/cInteractionManager.h"
#include "../gui/windows/GuiWindow.h"

#include "../utils/Logger.h"

// forward declaration (cannot include here, causing cyclic dependency, compile errors)
class StateRunner;

class MainMenuState : public State {
	public:
		MainMenuState() {
			guiWindow = NULL;
			interactionManager = NULL;
		}

		~MainMenuState() {
			delete guiWindow;
			delete interactionManager;
			Logger::getInstance()->debug("destructor State");
		}

		void draw();
		void manageTime();
		void updateState(StateRunner * stateRunner);

		void setGuiWindow(GuiWindow * guiWindow) {
			assert(guiWindow);
			this->guiWindow = guiWindow;
		}

		void setInteractionManager(cInteractionManager * interactionManager) {
			assert(interactionManager);
			this->interactionManager = interactionManager;
		}

	protected:

	private:
		GuiWindow * guiWindow;

		// interact with stuff (mouse/keyboard)
		cInteractionManager * interactionManager;

};

#endif /* MAIN_MENU_STATE_H_ */
