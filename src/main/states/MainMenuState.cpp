#include "MainMenuState.h"

void MainMenuState::draw() {
	screen->draw(mouse->getBitmap(), mouse->getX(), mouse->getY());
}

void MainMenuState::update() {
	mouse->updateState();

	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


