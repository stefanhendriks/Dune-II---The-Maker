#include "PlayingState.h"

void PlayingState::draw() {
	screen->draw(map->getBitmap(), 0, 0);
	viewPort->draw(screen, 0, 0);
	screen->draw(mouse->getBitmap(), mouse->getX(), mouse->getY());
}

void PlayingState::update() {
	mouse->updateState();

	// manipuleer camera/map

	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


