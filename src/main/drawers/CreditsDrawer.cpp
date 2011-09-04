/*
 * CreditsDrawer.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

#include "math.h"

CreditsDrawer::CreditsDrawer(cPlayer *thePlayer) {
	assert(thePlayer != NULL);
	player = thePlayer;
	bmp = NULL;
	memset(offset_credit, 0, sizeof(offset_credit));
	memset(offset_direction, 0, sizeof(offset_direction));
	initial = true;
	soundType = -1;
	rollSpeed = 0.0F;
	soundsMade = 0;
	newCreditsToDraw = 0;
	previousDrawnCredits = 0;
}

CreditsDrawer::~CreditsDrawer() {
	player = NULL;
}

void CreditsDrawer::destroy() {
	if (bmp) {
		destroy_bitmap(bmp);
		bmp = NULL;
	}
	player = NULL;
}

void CreditsDrawer::setCreditsOfPlayer() {
	setCredits((int) (player->credits));
}

void CreditsDrawer::setCredits(int amount) {
	initial = true;
	previousDrawnCredits = amount;
	newCreditsToDraw = amount;
	previousCreditsRolledTo = amount;
	memset(offset_credit, 0, sizeof(offset_credit));
	memset(offset_direction, 0, sizeof(offset_direction));
}

void CreditsDrawer::initState() {
	soundsMade = 0;

	memset(offset_credit, 0, sizeof(offset_credit));
	memset(offset_direction, 0, sizeof(offset_direction));

	// determine new currentCredits
	previousDrawnCredits = newCreditsToDraw;
}

int CreditsDrawer::getNewCreditsToDraw() {
	int creditsOfPlayer = (int) player->credits;

	if (creditsOfPlayer == previousDrawnCredits) {
		return previousDrawnCredits;
	}

	// credits of player differ from the last known (drawn) credits
	int diff = creditsOfPlayer - previousDrawnCredits;

	// when big difference, slice it up in pieces so we
	// have a more fluent transition

	if (diff > 200) {
		return previousDrawnCredits + (diff / 8);
	} else if (diff > 20) {
		return previousDrawnCredits + (diff / 4);
	}
	return creditsOfPlayer;
}

int CreditsDrawer::getSoundType() {
	if (newCreditsToDraw > previousDrawnCredits) {
		return SOUND_CREDITUP;
	} else if (newCreditsToDraw < previousDrawnCredits) {
		return SOUND_CREDITDOWN;
	}
	return -1;
}

void CreditsDrawer::slowdownRollSpeed() {
	if (rollSpeed > 0.0F) {
		rollSpeed -= 0.005;
	}
	if (rollSpeed < 0.0F) {
		rollSpeed = 0.0F;
	}
}

void CreditsDrawer::speedUpRollSpeed() {
	if (rollSpeed < 3.0F) {
		rollSpeed += 0.005;
	}
	if (rollSpeed > 3.0F) {
		rollSpeed = 3.0F;
	}
}

// timer based method
void CreditsDrawer::think() {
	if (hasDrawnCurrentCredits() || initial) {
		initState();

		int creditsOfPlayer = (int) player->credits;
		newCreditsToDraw = getNewCreditsToDraw();
		if (newCreditsToDraw == creditsOfPlayer) {
			previousCreditsRolledTo = creditsOfPlayer;
		}
		soundType = getSoundType();
		initial = false;
		slowdownRollSpeed();
	} else {
		int thinkRollingSpeed = 0;

		TIMER_money++;
		if (TIMER_money > thinkRollingSpeed) {
			TIMER_money = 0;
			speedUpRollSpeed();
		}

		thinkAboutIndividualCreditOffsets();
	}


}

// this will basically compare 2 credits values, and per offset determine whether
// it has to go 'up' or 'down'
int CreditsDrawer::getScrollingDirectionForOffset() {
    if(newCreditsToDraw > previousDrawnCredits) {
        return SCROLLING_DIRECTION_UP;
    }
    return SCROLLING_DIRECTION_DOWN;
}

bool CreditsDrawer::isOffsetDirectionSet(int id) {
	return offset_direction[id] != 0;
}

void CreditsDrawer::thinkAboutIndividualCreditOffsets() {
	char newCreditsAsChar[9];
	char previousDrawnCreditsAsChar[9];
	memset(newCreditsAsChar, 0, sizeof(newCreditsAsChar));
	memset(previousDrawnCreditsAsChar, 0, sizeof(previousDrawnCreditsAsChar));
	sprintf(newCreditsAsChar, "%d", newCreditsToDraw);
	sprintf(previousDrawnCreditsAsChar, "%d", previousDrawnCredits);

	for (int i = 0; i < 6; i++) {
		int newCreditDrawId = getCreditDrawIdByChar(newCreditsAsChar[i]);
		int previousCreditDrawId = getCreditDrawIdByChar(previousDrawnCreditsAsChar[i]);

		if (!isOffsetDirectionSet(i)) {
			if (newCreditDrawId == CREDITS_NONE || previousCreditDrawId == CREDITS_NONE) {
				// either of the 2 is unidentifiable. Do nothing
				offset_direction[i] = SCROLLING_DIRECTION_NONE;
				continue; // next
			}

			offset_direction[i] = getScrollingDirectionForOffset();

			if (newCreditDrawId == previousCreditDrawId) {
				offset_direction[i] = SCROLLING_DIRECTION_NONE;
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
			previousDrawnCreditsAsChar[i] = newCreditsAsChar[i];
			previousDrawnCredits = atoi(previousDrawnCreditsAsChar);
		} else {
			offset_credit[i] += 1.0F * rollSpeed;
		}
	}
}

void CreditsDrawer::draw() {
	if (bmp == NULL) {
		bmp = create_bitmap(120, 17);
	}
	clear_bitmap(bmp);
	clear_to_color(bmp, makecol(255, 0, 255));

	drawCurrentCredits();
	drawPreviousCredits();

	draw_sprite(bmp_screen, bmp, game.getScreenResolution()->getWidth() - 120, 8);
}

int CreditsDrawer::getXDrawingOffset(int amount) {
	int offset = 1;
	// TODO: make it return values, instead of iffing though everything.
	if (amount > 0)		offset = 4;
	if (amount < 10) 	offset = 5;
	if (amount > 99) 	offset = 3;
	if (amount > 999) 	offset = 2;
	if (amount > 9999) 	offset = 1;
	if (amount > 99999) offset = 0;
	return offset;
}

int CreditsDrawer::getCreditDrawIdByChar(char c) {
	if (c == '0')
		return CREDITS_0;
	if (c == '1')
		return CREDITS_1;
	if (c == '2')
		return CREDITS_2;
	if (c == '3')
		return CREDITS_3;
	if (c == '4')
		return CREDITS_4;
	if (c == '5')
		return CREDITS_5;
	if (c == '6')
		return CREDITS_6;
	if (c == '7')
		return CREDITS_7;
	if (c == '8')
		return CREDITS_8;
	if (c == '9')
		return CREDITS_9;
	return CREDITS_NONE;
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawCurrentCredits() {
	if (newCreditsToDraw < 0)
		newCreditsToDraw = 0;

	char credits[9];
	memset(credits, 0, sizeof(credits));
	sprintf(credits, "%d", newCreditsToDraw);

	int offset = getXDrawingOffset(newCreditsToDraw);

	for (int i = 0; i < 6; i++) {
		// the actual position to draw on is: ((game.screen_x - 120) +
		//        screen        -  14 + (6 digits (each 20 pixels due borders))
		int dx = ((offset + i) * 20);
		int dy = 0;

		// the Y is per credit direction
		if (offset_direction[i] == 1) {
			// up, means it comes from 'below' and the offset_credit is the 'coming up' part.
			dy = 20 - (int) offset_credit[i];
		} else if (offset_direction[i] == 2) {
			dy = -20 + (int) offset_credit[i];
		}

		int nr = getCreditDrawIdByChar(credits[i]);

		if (nr != CREDITS_NONE) {
			draw_sprite(bmp, (BITMAP *) gfxdata[nr].dat, dx, dy);
		}
	}
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawPreviousCredits() {
	if (previousDrawnCredits < 0)
		previousDrawnCredits = 0;

	char credits[9];
	memset(credits, 0, sizeof(credits));
	sprintf(credits, "%d", previousDrawnCredits);

	int offset = getXDrawingOffset(previousDrawnCredits);

	for (int i = 0; i < 6; i++) {
		// the actual position to draw on is: ((game.screen_x - 120) +
		//        screen        -  14 + (6 digits (each 20 pixels due borders))
		int dx = ((offset + i) * 20);
		int dy = 0;

		// the Y is per credit direction
		if (offset_direction[i] == 1) {
			// up, means it comes from 'below' and the offset_credit is the 'coming up' part.
			dy = (int) offset_credit[i] * -1;
		} else if (offset_direction[i] == 2) {
			dy = (int) offset_credit[i];
		}
		int nr = getCreditDrawIdByChar(credits[i]);

		if (nr != CREDITS_NONE) {
			draw_sprite(bmp, (BITMAP *) gfxdata[nr].dat, dx, dy);
		}
	}
}

// the think method will gradually update the currentCredits array so that it is 'the same'
bool CreditsDrawer::hasDrawnCurrentCredits() {
	return previousDrawnCredits == newCreditsToDraw;
}

// UNCOMMENT THIS IF YOU WANT PER CREDIT AN UP/DOWN ANIMATION, move this piece into
// thinkAboutIndividualCreditOffsets
//
//			if (currentId > previousId) {
//				offset_direction[i] = 1; // UP
//			} else if (currentId < previousId) {
//				offset_direction[i] = 2; // DOWN
//			}
