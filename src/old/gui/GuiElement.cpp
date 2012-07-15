#include "GuiElement.h"

#include "../utils/Logger.h"

GuiElement::~GuiElement() {
	Logger::getInstance()->debug("destructor GuiElement");
}


void GuiElement::draw() {
	// implemented in derived classes
}
