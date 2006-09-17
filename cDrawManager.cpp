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
		switch (game.getState()) {
		case GAME_PLAYING: 
			break;
		case GAME_BRIEFING:			
			Mentat->draw();
			break;
		case GAME_SETUPSKIRMISH:			
			break;
		case GAME_MENU:			
			break;
		case GAME_REGION:			
			break;
		case GAME_HOUSE:			
			break;
		case GAME_TELLHOUSE:
			Mentat->draw();
			break;
		case GAME_WINNING:			
			break;
		case GAME_LOSING:			
			break;
		case GAME_WINBRIEF:
			Mentat->draw();
			break;
		case GAME_LOSEBRIEF:
			Mentat->draw();
			break;
	}
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

