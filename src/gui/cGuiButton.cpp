#include <sstream>
#include <string>

#include "../gameobjects/cRectangle.h"
#include "cGuiShape.h"
#include "cGuiButton.h"

cGuiButton::cGuiButton(cRectangle * rect, std::string theLabel) : cGuiShape(rect) {
}

cGuiButton::~cGuiButton() {
}

