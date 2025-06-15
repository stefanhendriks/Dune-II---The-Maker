#include "cPalace.h"

#include "definitions.h"

cPalace::cPalace() {
    // other variables (class specific)

}

int cPalace::getType() const {
    return PALACE;
}

void cPalace::thinkFast() {
    cAbstractStructure::thinkFast();
}

void cPalace::think_animation() {
    // a palace does not animate, so when set, set it back to false
    if (isAnimating()) {
        setAnimating(false);
    }
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cPalace::think_guard() {

}
