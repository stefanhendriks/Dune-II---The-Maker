#include "cWor.h"

#include "definitions.h"

cWor::cWor() {
    // other variables (class specific)
}

int cWor::getType() const {
    return WOR;
}

void cWor::thinkFast() {
    // think like base class
    cAbstractStructure::thinkFast();
}

void cWor::think_animation() {
    // a wor does not animate, so when set, set it back to false
    if (isAnimating()) {
        setAnimating(false);
    }

    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cWor::think_guard() {

}
