#include "cMouseState.h"
// until we have our own data structure to abstract Allegro away, we have tight coupling here
#include "data/gfxdata.h"

#include <cassert>

cMouseState::cMouseState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    m_player(player),
    m_context(context),
    m_mouse(mouse),
    m_mouseTile(MOUSE_NORMAL)
{
    assert(player!=nullptr);
    assert(context!=nullptr);
    assert(mouse!=nullptr); 
}