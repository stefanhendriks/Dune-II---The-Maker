#include "d2tmh.h"

cDrawManager::cDrawManager() {

}

/** Only function public **/
void cDrawManager::draw() {
	drawEverythingOnBuffer();
	drawBufferOnScreen();	
}



void cDrawManager::drawEverythingOnBuffer() {
	drawByGameState();
	drawMouse();
}

void cDrawManager::drawByGameState() {

}

void cDrawManager::drawMouse() {
	Mouse.draw();
}

/**
	Function responsible for drawing the entire buffer
	onto the screen buffer.
**/
void cDrawManager::drawBufferOnScreen() {
	
}

