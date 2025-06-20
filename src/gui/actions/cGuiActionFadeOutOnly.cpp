#include "cGuiActionFadeOutOnly.h"

#include "d2tmc.h"

void cGuiActionFadeOutOnly::execute()
{
    game.initiateFadingOut();
}