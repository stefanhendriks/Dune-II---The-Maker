#include "MainMenuState.h"

void MainMenuState::draw() {
	mouse->draw(screen->getBitmap());
}

void MainMenuState::update() {
	mouse->updateState();

	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


