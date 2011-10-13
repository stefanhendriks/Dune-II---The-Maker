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

#define IDEAL_FPS	60

class State {
	public:
		State() {
			guiWindow = NULL;
			timeManager = NULL;
			interactionManager = NULL;
			restManager = NULL;
		}

		~State() {
			delete guiWindow;
			delete timeManager;
			delete restManager;
			delete interactionManager;
		}

		void updateState() {} ; // concrete state objects implement this function

		void run() {
			assert(guiWindow);
			timeManager->processTime();
			updateState();
			restManager->giveCpuSomeSlackIfNeeded();

			// interactionManager->interactWithKeyboard();
			// OR: interact with everything, or move to updateState ? (updateState does mouse related updating stuff)

			// shakeScreenAndBlitBuffer();
			// blit the screen (screenBlitter?) either shaky or not, depending
			// on action on battlefield.
			//
		};

		void setGuiWindow(GuiWindow * guiWindow) { this->guiWindow = guiWindow; }
		void setTimeManager(cTimeManager * timeManager) { this->timeManager = timeManager; }
		void setRestManager(RestManager * restManager) { this->restManager = restManager; }
		void setInteractionManager(cInteractionManager * interactionManager) { this->interactionManager = interactionManager; }

	protected:

	private:
		GuiWindow * guiWindow;

		RestManager * restManager;

		cTimeManager * timeManager;

		// interact with stuff (mouse/keyboard)
		cInteractionManager * interactionManager;

};

#endif /* STATE_H_ */
