#ifndef MAIN_MENU_STATE_H_
#define MAIN_MENU_STATE_H_

#include <assert.h>
#include <cstddef>

#include "State.h"

#include "../utils/Logger.h"

class MainMenuState : public State {
	public:
		MainMenuState(Screen * screen, Mouse * mouse) : State(screen, mouse) {
		
		}

		~MainMenuState() {
			Logger::getInstance()->debug("Destroying main menu state");
		}

		void draw();
		void update();

	protected:

	private:
		

};

#endif /* MAIN_MENU_STATE_H_ */
