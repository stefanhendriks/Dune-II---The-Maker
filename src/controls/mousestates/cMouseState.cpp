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
    my_assert(player!=nullptr);
    my_assert(context!=nullptr);
    my_assert(mouse!=nullptr); 
}