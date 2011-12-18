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

#define IDEAL_FPS	60

// forward declaration (cannot include here, causing cyclic dependency, compile errors)
class StateRunner;

class State {
	public:
		State() {
			quitGame = false;
		}

		virtual ~State() {
			quitGame = false;
		}

		virtual void draw() = 0;
		virtual void manageTime() = 0;
		virtual void updateState(StateRunner * stateRunner) = 0;

		bool shouldQuitGame() { return quitGame; }
		void flagToQuitGame() { quitGame = true; }

	protected:

	private:

		bool quitGame;


};

#endif /* STATE_H_ */
