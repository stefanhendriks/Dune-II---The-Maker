/*
 * File:   cMouseDrawer.cpp
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

#include "../d2tmh.h"

cMouseDrawer::cMouseDrawer(cPlayer *thePlayer, cMouse *theMouse) {
	assert(thePlayer);
	assert(theMouse);
	player = thePlayer;
	mouse = theMouse;
	mouseToolTip = new cMouseToolTip(player, mouse);
}

cMouseDrawer::cMouseDrawer(const cMouseDrawer& orig) {
}

cMouseDrawer::~cMouseDrawer() {
	player = NULL;
	mouse = NULL;
	delete mouseToolTip;
}

void cMouseDrawer::draw() {
	int x = mouse->getX();
	int y = mouse->getY();

	// adjust coordinates of drawing according to the specific mouse sprite/tile
	if (mouse_tile == MOUSE_DOWN){
		y-=16;
    }
    else if (mouse_tile == MOUSE_RIGHT){
    	x-=16;
    }
    else if (mouse_tile == MOUSE_MOVE || mouse_tile == MOUSE_RALLY){
    	x-=16;
    	y-=16;
    }
    else if (mouse_tile == MOUSE_ATTACK){
    	x-=16;
    	y-=16;
    }
    else if (mouse_tile == MOUSE_REPAIR){
    	x-=16;
    	y-=16;
    }
    else if (mouse_tile == MOUSE_PICK){
    	x-=16;
    	y-=16;
    }
    else{
    }

	draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, x, y);
}

int cMouseDrawer::getDrawXToolTip(int width) {
	int x = mouse->getX() + 32;

	// correct drawing position so it does not fall off screen.
	int diffX = (x + width) - game.screen_x;
	if (diffX > 0) {
		x-= diffX;
	}
	return x;
}

int cMouseDrawer::getDrawYToolTip(int height) {
	int y = mouse->getY() + 32;

	// correct drawing position so it does not fall off screen.
	int diffY = (y + height) - game.screen_y;
	if (diffY > 0) {
		y -= diffY;
	}
	return y;
}

int cMouseDrawer::getWidthToolTip() {
	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		return 130;
	}

	return 0;
}

int cMouseDrawer::getHeightToolTip() {
	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		return 70;
	}

	return 0;
}

void cMouseDrawer::drawToolTip() {
	int height = getHeightToolTip();
	int width = getWidthToolTip();

	if (height <= 0|| width <= 0) {
		return;
	}

	int x = getDrawXToolTip(width);
	int y = getDrawYToolTip(height);

	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		cTextWriter * textWriter = new cTextWriter((x + 2), (y + 2), small_font, 12);

		cAbstractStructure * theStructure = context->getStructurePointerWhereMouseHovers();

		drawToolTipBackground();
		drawToolTipGeneralInformation(theStructure, textWriter);

//		// depending on structure type give more info
		if (theStructure->getType() == WINDTRAP) {
			cWindTrap * windTrap = dynamic_cast<cWindTrap*>(theStructure);
			drawToolTipWindTrapInformation(windTrap, textWriter);
		}
//
//		if (theStructure->getType() == SILO || theStructure->getType() == REFINERY) {
//			textY += 14;
//			int maxSpice = player->max_credits;
//			int currentSpice =  player->credits;
//			textDrawer.drawTextWithTwoIntegers(textX, textY, "Spice usage : %d/%d", currentSpice, maxSpice);
//		}
	}
}

void cMouseDrawer::drawToolTipBackground() {
	cGameControlsContext * context = player->getGameControlsContext();

	int width, height;
	width=height=0;

	if (context->isMouseOverStructure()) {
		width=130;
		height=70;
	} else {
		// do not draw
		return;
	}

	int x = getDrawXToolTip(width);
	int y = getDrawYToolTip(height);

//	fblend_rect_trans(bmp_screen, x, y, width, height, makecol(0,0,0), 128);
	rect(bmp_screen, x, y, x+(width-1), y + (height-1), makecol(255,255,255));
	fblend_rect_trans(bmp_screen, x + 4, y + 4, width, height, makecol(0,0,0), 128);
	fblend_rect_trans(bmp_screen, x, y, width, height, player->getMinimapColor(), 128);
}


void cMouseDrawer::drawToolTipGeneralInformation(cAbstractStructure * theStructure, cTextWriter *textWriter) {
	assert(theStructure);
	assert(textWriter);
	s_Structures structureType = theStructure->getS_StructuresType();
	textWriter->write(structureType.name, makecol(255, 255, 0));
	textWriter->writeWithTwoIntegers("Hitpoints : %d/%d", theStructure->getHitPoints(), theStructure->getMaxHP());
	textWriter->writeWithOneInteger("%d%% protected", (100-theStructure->getPercentageNotPaved()));
}

void cMouseDrawer::drawToolTipWindTrapInformation(cWindTrap * theWindTrap, cTextWriter *textWriter) {
	assert(theWindTrap);
	assert(textWriter);
	int powerOut = theWindTrap->getPlayer()->has_power;
	int powerUse = theWindTrap->getPlayer()->use_power;
	textWriter->writeWithTwoIntegers("Total usage   : %d/%d", powerUse, powerOut);
	textWriter->writeWithTwoIntegers("Windtrap outage: %d/%d", theWindTrap->getPowerOut(), theWindTrap->getMaxPowerOut());
}

void cMouseDrawer::drawToolTipSiloInformation(cSpiceSilo * theSpiceSilo, cTextWriter *textWriter) {

}
