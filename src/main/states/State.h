/**
 * Abstract State class. Each state has at the bare minimum a window (gui) to draw, and some functions to update
 * the state. These update functions are time and non-time related. The state runner is responsible for calling
 * the right methods in the right order.
 *
 * This class is an implementation of the State Patterns, where state is not used within if-then-else statements,
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
			timeManager = NULL;
			interactionManager = NULL;
			restManager = NULL;
			quitGame = false;
			mouse = NULL;
		}

		~State() {
			delete guiWindow;
			delete timeManager;
			delete restManager;
			delete interactionManager;
			Logger::getInstance()->debug("destructor State");
		}

		void updateState(StateRunner * stateRunner) {

		} ; // concrete state objects implement this function

		void run(StateRunner * stateRunner) {
			assert(guiWindow);
			timeManager->processTime();
			restManager->giveCpuSomeSlackIfNeeded();
			mouse->updateState();

			updateState(stateRunner);

			guiWindow->interact();
			guiWindow->draw();

			// interactionManager->interactWithKeyboard();
			// OR: interact with everything, or move to updateState ? (updateState does mouse related updating stuff)

			// TODO: interaction with keyboard needs to be thought out, for now add ESQ to quit game
			if (key[KEY_ESC]) {
				flagToQuitGame();
			}

			// shakeScreenAndBlitBuffer();
			// blit the screen (screenBlitter?) either shaky or not, depending
			// on action on battlefield.
			//
		}

		void setGuiWindow(GuiWindow * guiWindow) {
			assert(guiWindow);
			this->guiWindow = guiWindow;
		}

		void setTimeManager(cTimeManager * timeManager) {
			assert(timeManager);
			this->timeManager = timeManager;
		}

		void setRestManager(RestManager * restManager) {
			assert(restManager);
			this->restManager = restManager;
		}

		void setInteractionManager(cInteractionManager * interactionManager) {
			assert(interactionManager);
			this->interactionManager = interactionManager;
		}

		void setMouse(Mouse * mouse) {
			assert(mouse);
			this->mouse = mouse;
		}

		bool shouldQuitGame() { return quitGame; }
		void flagToQuitGame() { quitGame = true; }

	protected:

	private:
		GuiWindow * guiWindow;

		RestManager * restManager;

		cTimeManager * timeManager;

		// interact with stuff (mouse/keyboard)
		cInteractionManager * interactionManager;

		Mouse * mouse;

		bool quitGame;

};

#endif /* STATE_H_ */
