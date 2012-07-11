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
		MainMenuState(Screen * screen, Mouse * mouse) : State(screen, mouse) {
			guiWindow = NULL;
			interactionManager = NULL;
		}

		~MainMenuState() {
			Logger::getInstance()->debug("Destroying main menu state");
		}

		void draw();
		void update();

	protected:

	private:
		GuiWindow * guiWindow;

		// interact with stuff (mouse/keyboard)
		cInteractionManager * interactionManager;


};

#endif /* MAIN_MENU_STATE_H_ */
