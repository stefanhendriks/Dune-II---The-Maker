#include "game/cGameInterface.h"
#include "data/gfxaudio.h"
#include "game/cGame.h"
#include "utils/cLog.h"

#include <format>

cGameInterface::cGameInterface(cGame* game)
    : m_igame(game)
{
}

void cGameInterface::prepareMentatToTellAboutHouse(int house) const
{
    switch(house) {
    case ATREIDES:
            m_igame->prepareMentatToTellAboutHouse(ATREIDES);
            m_igame->playSound(SOUND_ATREIDES);
            break;
    case HARKONNEN:
            m_igame->prepareMentatToTellAboutHouse(HARKONNEN);
            m_igame->playSound(SOUND_HARKONNEN);
            break;
    case ORDOS:
            m_igame->prepareMentatToTellAboutHouse(ORDOS);
            m_igame->playSound(SOUND_ORDOS);
            break;
    default:
        cLogger::getInstance()->log(LOG_WARN,COMP_GAMERULES, "House", std::format("cGameInterface::prepareMentatToTellAboutHouse() was called with an invalid house value: {}", house));
        break;
    }
    m_igame->setNextStateToTransitionTo(GAME_TELLHOUSE);
    m_igame->initiateFadingOut();
}

void cGameInterface::setTransitionToWithFadingOut(int newState) const
{
        m_igame->setNextStateToTransitionTo(newState);
        m_igame->initiateFadingOut();
}
