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
	rectfill(bmp_screen, x, y, (x+ 100), (y+100), makecol(255,255,255));
}


