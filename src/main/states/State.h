/**
 * Abstract State class. Each state has at the bare minimum a window (gui) to draw, and some functions to update
 * the state. These update functions are time and non-time related. The state runner is responsible for calling
 * the right methods in the right order.
 *
 * This class is an implementation of the State Pattern, where state is not used within if-then-else statements,
 * but as classes themselves. The only place where an if-then-else structure is used, is within the StateFactory.
 *
 */
#ifndef STATE_H_
#define STATE_H_

#include <assert.h>
#include <cstddef>

#include "../gui/windows/GuiWindow.h"

#include "../managers/RestManager.h"
#include "../managers/cInteractionManager.h"
#include "../utils/cTimeManager.h" // <-- oh oh, this already points out something is wrong
#include "../utils/Logger.h"
#include "../controls/Mouse.h"

#define IDEAL_FPS	60

// forward declaration (cannot include here, causing cyclic dependency, compile errors)
class StateRunner;

class State {
	public:
		State() {
			guiWindow = NULL;
			interactionManager = NULL;
			quitGame = false;
		}

		~State() {
			delete guiWindow;
			delete interactionManager;
			Logger::getInstance()->debug("destructor State");
		}

		void draw() {
			guiWindow->draw();
		}

		void manageTime() {

		}

		void updateState(StateRunner * stateRunner) {
			guiWindow->interact();
			if (key[KEY_ESC]) {
				flagToQuitGame();
			}

		}

		void setGuiWindow(GuiWindow * guiWindow) {
			assert(guiWindow);
			this->guiWindow = guiWindow;
		}

		void setInteractionManager(cInteractionManager * interactionManager) {
			assert(interactionManager);
			this->interactionManager = interactionManager;
		}

		bool shouldQuitGame() { return quitGame; }
		void flagToQuitGame() { quitGame = true; }

	protected:

	private:
		GuiWindow * guiWindow;

		// interact with stuff (mouse/keyboard)
		cInteractionManager * interactionManager;

		bool quitGame;

};

#endif /* STATE_H_ */
