#include "d2tmh.h"

cDrawManager::cDrawManager() {
	buffer = MMEngine->createSurface(game.screen_x, game.screen_y);
	MMEngine->clearSurface(buffer);
}

/** Only function public responsible for drawing everything **/
void cDrawManager::draw() {
	drawEverythingOnBuffer();
	drawBufferOnScreen();	
}

/** Needed for DrawManager **/
MME_SURFACE cDrawManager::getBuffer() {
	return buffer;
}

void cDrawManager::drawEverythingOnBuffer() {
	drawByGameState();
	drawMouse();
}

void cDrawManager::drawByGameState() {
	/** TODO: Draw stuff by game state **/
}


void cDrawManager::drawMouse() {
	Mouse.draw();
}

/**
	Function responsible for drawing the entire buffer
	onto the screen buffer.
**/
void cDrawManager::drawBufferOnScreen() {
//	MMEngine->doBlit(buffer, MME_SCREENBUFFER, 0, 0);
}

