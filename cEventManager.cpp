#include "d2tmh.h"

cEventManager::cEventManager() {

}

/** 
	Responsible for catching and eventualy reacting upon events
	if nescesary 
*/
void cEventManager::handleEvents() {
	pollMouse();
	pollKeyboard();
}


/** Poll keyboard keys **/
void cEventManager::pollKeyboard() {
	
}

/** Poll mouse coordinates **/
void cEventManager::pollMouse() {
	Mouse.setPoll(MMEngine->getMouseX(), 
				  MMEngine->getMouseY(), 
				  MMEngine->getMouseZ());
}