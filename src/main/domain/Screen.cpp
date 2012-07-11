#include "Screen.h"

void Screen::blitScreenBufferToScreen() {
	blit(buffer, screen, 0, 0, 0, 0, screenResolution->getWidth(), screenResolution->getHeight());
}

BITMAP * Screen::getBuffer() {
	return this->buffer;
}

ScreenResolution * Screen::getScreenResolution() {
	return this->screenResolution;
}

/**
 * if (TIMER_shake == 0) {
		TIMER_shake = -1;
	}
	// blit on screen

	if (TIMER_shake > 0) {
		// the more we get to the 'end' the less we 'throttle'.
		// Structure explosions are 6 time units per cell.
		// Max is 9 cells (9*6=54)
		// the max border is then 9. So, we do time / 6
		if (TIMER_shake > 69)
			TIMER_shake = 69;

		int offset = TIMER_shake / 5;
		if (offset > 9)
			offset = 9;

		shake_x = -abs(offset / 2) + rnd(offset);
		shake_y = -abs(offset / 2) + rnd(offset);

		blit(bmp_screen, bmp_throttle, 0, 0, 0 + shake_x, 0 + shake_y, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
		blit(bmp_throttle, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
	} else {
		// when fading
		if (iAlphaScreen == 255)
			blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
		else {
			BITMAP *temp = create_bitmap(game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
			assert(temp != NULL);
			clear(temp);
			fblend_trans(bmp_screen, temp, 0, 0, iAlphaScreen);
			blit(temp, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
			destroy_bitmap(temp);
		}

	}
 */
