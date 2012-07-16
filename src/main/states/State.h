#pragma once
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

#include "../domain/Mouse.h"
#include "../domain/Screen.h"

#include "../timers.h"

#define IDEAL_FPS	60

class State {
	public:
		State(Screen * screen, Mouse * mouse) {
			assert(screen);
			assert(mouse);
			this->screen = screen;
			this->mouse = mouse;
			quitGame = false;
		}

		virtual ~State() {
			quitGame = false;
		}

		void run() {
			screen->clearBuffer();
			rest(10);		// give cpu slack (TODO: make this more smart, like the RestManager did it)
			processTime();	// perhaps this can be merged with...
			update();		// ... this function
			draw();			// <-- implemented by all states (draw state)
			screen->blitScreenBufferToScreen();
		}

		void processTime() {
			this->gameticks = timerGameTimer;
			this->seconds = timerPerSecond;

			// TODO: do stuff with time here (see TimerManager)
			timerGameTimer = this->gameticks;
			timerPerSecond = this->seconds;
		};
		
		virtual void update() = 0;
		virtual void draw() = 0;

		bool shouldQuitGame() { return quitGame; }
		void flagToQuitGame() { quitGame = true; }

	protected:
		Screen * screen;
		Mouse * mouse;

	private:


		bool quitGame;

		int seconds;
		int gameticks;


};

#endif /* STATE_H_ */
