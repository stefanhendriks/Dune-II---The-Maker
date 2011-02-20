/*
 * CreditsDrawer.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

CreditsDrawer::CreditsDrawer(cPlayer *thePlayer) {
	assert(thePlayer != NULL);
	player = thePlayer;
	bmp = NULL;
	memset(offset_credit, 0, sizeof(offset_credit));
	memset(offset_direction, 0, sizeof(offset_direction));
	initial = true;
	soundType = -1;
	rollSpeed = 0.0F;
	rolled = 0;
	soundsMade = 0;
	currentCredits = 0;
	previousCredits = 0;
}

CreditsDrawer::~CreditsDrawer() {
	// TODO Auto-generated destructor stub
}

void CreditsDrawer::setCredits() {
	setCredits((int)player->credits);
}

void CreditsDrawer::setCredits(int amount) {
	initial = true;
	previousCredits = amount;
	currentCredits = amount;
	memset(offset_credit, 0, sizeof(offset_credit));
	memset(offset_direction, 0, sizeof(offset_direction));
}

// timer based method
void CreditsDrawer::think() {

	if (hasDrawnCurrentCredits() || initial) {
		soundsMade = 0;

		memset(offset_credit, 0, sizeof(offset_credit));
		memset(offset_direction, 0, sizeof(offset_direction));

		// determine new currentCredits
		// TODO: make it 'roll' instead of 'jump' to the newest credits?
		previousCredits = currentCredits;
		int newCurrentCredits = (int)player->credits;

		if (newCurrentCredits != previousCredits) {
			int diff = newCurrentCredits - previousCredits;
			// when big difference, slice it up in pieces so we
			// have a more fluent transition
			if (diff > 200) {
				currentCredits = previousCredits + (diff / 8);
			} else if (diff > 20) {
				currentCredits = previousCredits + (diff / 4);
			} else {
				currentCredits = newCurrentCredits;
			}
		}

		// decide what sound to play when done
		if (currentCredits > previousCredits) {
			soundType = SOUND_CREDITUP;
		} else if (currentCredits < previousCredits){
			soundType = SOUND_CREDITDOWN;
		} else {
			soundType = -1;
		}

		TIMER_money++;
		if (TIMER_money > 20) {
			if (rollSpeed > 0.0F) {
				rollSpeed -= 0.005;
			}
			if (rollSpeed < 0.0F) {
				rollSpeed = 0.0F;
				TIMER_money = 0;
			}
		}

		initial = false;
	} else {
		TIMER_money = 0;
		if (rollSpeed < 3.0F) {
			rollSpeed += 0.005;
		}
		thinkAboutIndividualCreditOffsets();
	}
}

// this will basically compare 2 credits values, and per offset determine whether
// it has to go 'up' or 'down'
void CreditsDrawer::thinkAboutIndividualCreditOffsets() {
	char current_credits[9];
	char previous_credits[9];
	memset(current_credits, 0, sizeof(current_credits));
	memset(previous_credits, 0, sizeof(previous_credits));
	sprintf(current_credits, "%d", currentCredits);
	sprintf(previous_credits, "%d", previousCredits);

	for (int i = 0; i < 6; i++) {
		int currentId = getCreditDrawId(current_credits[i]);
		int previousId = getCreditDrawId(previous_credits[i]);

		if (offset_direction[i] == 0) {
			if (currentId == CREDITS_NONE || previousId == CREDITS_NONE) {
				// either of the 2 is unidentifiable. Do nothing
				offset_direction[i] = 3;
				continue; // next
			}

			// UP/DOWN ANIMATION: Is over the entire sum of money, so all 'credits' move the same
			// way.
			if (currentCredits > previousCredits) {
				offset_direction[i] = 1;
			} else if (previousCredits > currentCredits) {
				offset_direction[i] = 2;
			}

			// when the same, do not animate
			if (currentId == previousId) {
				offset_direction[i] = 3;
			}

		}

		// always 'add' here, the draw routine knows how to handle this for up/down movement.
		if (offset_credit[i] > 18.0F) {
			//offset_credit[i] = 18.1F; // so we do not keep matching our IF :)
			offset_credit[i] = 0.0F; // so we do not keep matching our IF :)
			if (soundsMade < 7) {
				play_sound_id(soundType, -1);
				soundsMade++;
			}
			// it is fully 'moved'. Now update the array.
			previous_credits[i] = current_credits[i];
			previousCredits = atoi(previous_credits);
		} else {
			offset_credit[i] += 1.0F * rollSpeed;
		}
	}
}

/**
 * Create bitmap to draw credits
 */
void CreditsDrawer::draw() {
	if (bmp == NULL) {
		bmp = create_bitmap(120, 17);
	}
	clear_bitmap(bmp);
	clear_to_color(bmp, makecol(255, 0, 255)); // transparency

	drawCurrentCredits();
	drawPreviousCredits();

	// draw bmp on screen
	draw_sprite(bmp_screen, bmp, game.getScreenResolution()->getWidth() - 120, 8);
//	draw_sprite(bmp_screen, bmp, mouse_x - 120, mouse_y - 8);
}

int CreditsDrawer::getXDrawingOffset(int amount) {
	int offset = 1;
	if (amount > 0)		offset = 4;
	if (amount < 10)	offset =  5;
	if (amount > 99)	offset =  3;
	if (amount > 999)	offset =  2;
	if (amount > 9999) 	offset =  1;
	if (amount > 99999) offset =  0;
	return offset;
}

int CreditsDrawer::getCreditDrawId(char c) {
	if (c == '0') return CREDITS_0;
	if (c == '1') return CREDITS_1;
	if (c == '2') return CREDITS_2;
	if (c == '3') return CREDITS_3;
	if (c == '4') return CREDITS_4;
	if (c == '5') return CREDITS_5;
	if (c == '6') return CREDITS_6;
	if (c == '7') return CREDITS_7;
	if (c == '8') return CREDITS_8;
	if (c == '9') return CREDITS_9;
	return CREDITS_NONE;
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawCurrentCredits() {
	if (currentCredits < 0) currentCredits = 0;

	char credits[9];
	memset(credits, 0, sizeof(credits));
	sprintf(credits, "%d", currentCredits);

	int offset = getXDrawingOffset(currentCredits);

	for (int i=0; i < 6; i++) {
		// the actual position to draw on is: ((game.screen_x - 120) +
		//        screen        -  14 + (6 digits (each 20 pixels due borders))
		int dx = ((offset + i) * 20);
		int dy = 0;

		// the Y is per credit direction
		if (offset_direction[i] == 1) {
			// up, means it comes from 'below' and the offset_credit is the 'coming up' part.
			dy = 20 - (int)offset_credit[i];
		} else if (offset_direction[i] == 2) {
			dy = -20 + (int)offset_credit[i];
		}

		int nr = getCreditDrawId(credits[i]);

		if (nr != CREDITS_NONE) {
			draw_sprite(bmp, (BITMAP *)gfxdata[nr].dat, dx, dy);
		}
	}
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawPreviousCredits() {
	if (previousCredits < 0) previousCredits = 0;

	char credits[9];
	memset(credits, 0, sizeof(credits));
	sprintf(credits, "%d", previousCredits);

	int offset = getXDrawingOffset(previousCredits);

	for (int i=0; i < 6; i++) {
		// the actual position to draw on is: ((game.screen_x - 120) +
		//        screen        -  14 + (6 digits (each 20 pixels due borders))
		int dx = ((offset + i) * 20);
		int dy = 0;

		// the Y is per credit direction
		if (offset_direction[i] == 1) {
			// up, means it comes from 'below' and the offset_credit is the 'coming up' part.
			dy = (int)offset_credit[i] * -1;
		} else if (offset_direction[i] == 2) {
			dy = (int)offset_credit[i];
		}
		int nr = getCreditDrawId(credits[i]);

		if (nr != CREDITS_NONE) {
			draw_sprite(bmp, (BITMAP *)gfxdata[nr].dat, dx, dy);
		}
	}
}

// the think method will gradually update the currentCredits array so that it is 'the same'
bool CreditsDrawer::hasDrawnCurrentCredits() {
	return previousCredits == currentCredits;
}


// UNCOMMENT THIS IF YOU WANT PER CREDIT AN UP/DOWN ANIMATION, move this piece into
// thinkAboutIndividualCreditOffsets
//
//			if (currentId > previousId) {
//				offset_direction[i] = 1; // UP
//			} else if (currentId < previousId) {
//				offset_direction[i] = 2; // DOWN
//			}
