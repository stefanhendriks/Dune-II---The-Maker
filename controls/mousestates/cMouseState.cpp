#include "cMouseState.h"
// until we have our own data structure to abstract Allegro away, we have tight coupling here
#include "../../data/gfxdata.h"

cMouseState::cMouseState(cGameControlsContext *context, cMouse *mouse) :
        context(context),
        mouse(mouse),
        mouseTile(MOUSE_NORMAL) {

}

cMouseState::~cMouseState() {

}
