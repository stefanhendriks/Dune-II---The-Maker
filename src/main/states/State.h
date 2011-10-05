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

class State {
	public:
		State(GuiWindow * theGuiWindow) {
			assert(theGuiWindow);
			guiWindow = theGuiWindow;
		}

		~State() {
			guiWindow = NULL;
		}

	protected:

	private:
		GuiWindow * guiWindow;
		// TODO: own time manager
};

#endif /* STATE_H_ */
