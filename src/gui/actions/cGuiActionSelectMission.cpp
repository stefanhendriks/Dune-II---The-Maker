#include "cGuiActionSelectMission.h"

#include "d2tmc.h"

cGuiActionSelectMission::cGuiActionSelectMission(int missionNr)
{
    m_missionNr = missionNr;
}

void cGuiActionSelectMission::execute()
{
    game.jumpToSelectYourNextConquestMission(m_missionNr);
    game.setNextStateToTransitionTo(GAME_REGION);
    game.initiateFadingOut();
}