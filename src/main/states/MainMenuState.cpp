#include "MainMenuState.h"

void MainMenuState::draw() {
	mouse->draw(screen->getBuffer());
}

void MainMenuState::update() {
	mouse->updateState();

	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


