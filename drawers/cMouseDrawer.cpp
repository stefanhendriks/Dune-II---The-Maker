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

void cMouseDrawer::drawToolTip() {
	int x = mouse->getX() + 32;
	int y = mouse->getY() + 32;

	cGameControlsContext * context = player->getGameControlsContext();

	int width, height;
	width=height=0;
	if (context->isMouseOverStructure()) {
		width=130;
		height=70;
	}

	// correct drawing position so it does not fall off screen.
	int diffX = (x + width) - game.screen_x;
	int diffY = (y + height) - game.screen_y;
	if (diffX > 0) {
		x-= diffX;
	}
	if (diffY > 0) {
		y -= diffY;
	}

	fblend_rect_trans(bmp_screen, x, y, width, height, makecol(0,0,0), 128);

	if (context->isMouseOverStructure()) {
		int structureId = context->getIdOfStructureWhereMouseHovers();
		cAbstractStructure * theStructure = structure[structureId];
		s_Structures structureType = structures[theStructure->getType()];

		cTextDrawer textDrawer;
		int textX = x + 2;
		int textY = y + 8;
		textDrawer.drawText(textX, textY, structureType.name);

		textY += 14;
		int currentHp = theStructure->getHitPoints();
		int maxHp = structureType.hp;
		textDrawer.drawTextWithTwoIntegers(textX, textY, "Hitpoints : %d/%d", currentHp, maxHp);

		// depending on structure type give more info
		if (theStructure->getType() == WINDTRAP) {
			textY += 14;
			int powerOut = player->has_power;
			int powerUse =  player->use_power;
			textDrawer.drawTextWithTwoIntegers(textX, textY, "Total usage   : %d/%d", powerUse, powerOut);

			textY += 14;
			cWindTrap * windTrap = dynamic_cast<cWindTrap*>(theStructure);
			powerOut = windTrap->powerOut();
			int maxOut = structureType.power_give;
			textDrawer.drawTextWithTwoIntegers(textX, textY, "Windtrap outage: %d/%d", powerOut, maxOut);
		}

		if (theStructure->getType() == SILO || theStructure->getType() == REFINERY) {
			textY += 14;
			int maxSpice = player->max_credits;
			int currentSpice =  player->credits;
			textDrawer.drawTextWithTwoIntegers(textX, textY, "Spice usage : %d/%d", currentSpice, maxSpice);
		}
	}
}


