#include "cMouseState.h"
// until we have our own data structure to abstract Allegro away, we have tight coupling here
#include "../../data/gfxdata.h"

cMouseState::cMouseState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    m_player(player),
    m_context(context),
    m_mouse(mouse),
    mouseTile(MOUSE_NORMAL)
{

}