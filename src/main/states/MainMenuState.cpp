#include "MainMenuState.h"

void MainMenuState::draw() {
	screen->draw(mouse->getBitmap(), mouse->getVector2D());
}

void MainMenuState::update() {
	mouse->updateState();

	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


