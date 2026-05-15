#include "cMouseState.h"
// until we have our own data structure to abstract Allegro away, we have tight coupling here
#include "data/gfxdata.h"

#include "include/cAssert.h"

cMouseState::cMouseState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    m_player(player),
    m_context(context),
    m_mouse(mouse),
    m_mouseTile(MOUSE_NORMAL)
{
    d2tm_assert(player!=nullptr);
    d2tm_assert(context!=nullptr);
    d2tm_assert(mouse!=nullptr); 
}