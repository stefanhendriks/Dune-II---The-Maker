#include "PlayingState.h"

void PlayingState::draw() {

	// draw map

	mouse->draw(screen->getBitmap());
}

void PlayingState::update() {
	mouse->updateState();

	// manipuleer camera/map


	if (key[KEY_ESC]) {
		this->flagToQuitGame();
	}
}


