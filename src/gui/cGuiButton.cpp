#include <sstream>
#include <string>

#include "cGuiShape.h"
#include "cGuiButton.h"

cGuiButton::cGuiButton() {
}

cGuiButton::cGuiButton(int theX, int theY, int theWidth, int theHeight, std::string theLabel) : cGuiShape(theX, theY, theWidth, theHeight) {
}

cGuiButton::~cGuiButton() {
}

