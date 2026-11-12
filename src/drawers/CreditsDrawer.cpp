/**
 * @file CreditsDrawer.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "CreditsDrawer.h"
#include "CreditsMath.hpp"
#include "include/sGameServices.h"
#include "utils/common.h"
#include "context/AudioContext.hpp"
#include "game/cGameSettings.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "gameobjects/players/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/cSoundPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"

#include <SDL3/SDL.h>
#include "include/cAssert.h"
#include "include/Texture.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

namespace {
// digits of the credits, cut off / padded with '\0' to 8 characters (what the old char[9] buffers held)
std::string creditsToDigits(int credits)
{
    std::string digits = std::to_string(credits);
    digits.resize(8, '\0');
    return digits;
}
}

CreditsDrawer::CreditsDrawer(GameContext* ctx, cGameSettings *settings, cPlayer *player) :
    m_player(player),
    m_ctx(ctx),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_gfxdata(ctx->getGraphicsContext()->gfxdata.get()),
    m_sdlDrawer(ctx->getSDLDrawer()),
    m_playCreditsSound(settings->isPlayCreditsSound())
{
    d2tm_assert(player!= nullptr);
    d2tm_assert(ctx != nullptr);
    d2tm_assert(settings != nullptr);

    std::fill(std::begin(m_offset_credit), std::end(m_offset_credit), 0.0f);
    std::fill(std::begin(m_offset_direction), std::end(m_offset_direction), 0);
    m_initial = true;
    m_soundType = audioID::invalid;
    m_rollSpeed = 0.0F;
    m_rolled = 0;
    m_soundsMade = 0;
    m_currentCredits = 0;
    m_previousCredits = 0;

    // center credits bar within sidebar
    int widthCreditsBar = (m_gfxinter->getSurface(CREDITS_BAR))->w;
    m_drawX = settings->getScreenW() - (cSideBar::SidebarWidthWithoutCandyBar / 2) - (widthCreditsBar / 2);
    m_drawY = 0;
}

void CreditsDrawer::serviceInit(sGameServices* services)
{
    m_audioContext = services->ctx->getAudioContext();
    m_creditUpSound = m_audioContext->indexOf("SOUND_CREDIT_UP");
    m_creditDownSound = m_audioContext->indexOf("SOUND_CREDIT_DOWN");
    m_mapCamera = services->mapCamera;
    m_objects = services->objects;
}

CreditsDrawer::~CreditsDrawer()
{
    // TODO Auto-generated destructor stub
}

void CreditsDrawer::setCredits()
{
    setCredits(m_player->getCredits());
}

void CreditsDrawer::setCredits(int amount)
{
    m_initial = true;
    m_previousCredits = amount;
    m_currentCredits = amount;
    std::fill(std::begin(m_offset_credit), std::end(m_offset_credit), 0.0f);
    std::fill(std::begin(m_offset_direction), std::end(m_offset_direction), 0);
}

// timer based method
void CreditsDrawer::thinkFast()
{

    if (hasDrawnCurrentCredits() || m_initial) {
        m_soundsMade = 0;

        std::fill(std::begin(m_offset_credit), std::end(m_offset_credit), 0.0f);
        std::fill(std::begin(m_offset_direction), std::end(m_offset_direction), 0);

        // determine new m_currentCredits
        // TODO: make it 'roll' instead of 'jump' to the newest credits?
        m_previousCredits = m_currentCredits;
        auto newCurrentCredits = m_player->getCredits();

        m_currentCredits = CreditsMath::nextDisplayedCredits(m_previousCredits, newCurrentCredits);

        // decide what sound to play when done
        if (m_currentCredits > m_previousCredits) {
            m_soundType = m_creditUpSound;
        }
        else if (m_currentCredits < m_previousCredits) {
            m_soundType = m_creditDownSound;
        }
        else {
            m_soundType = audioID::invalid;
        }

        m_TIMER_money++;
        if (m_TIMER_money > 20) {
            if (m_rollSpeed > 0.0F) {
                m_rollSpeed -= 0.005;
            }
            if (m_rollSpeed < 0.0F) {
                m_rollSpeed = 0.0F;
                m_TIMER_money = 0;
            }
        }

        m_initial = false;
    }
    else {
        m_TIMER_money = 0;
        if (m_rollSpeed < 3.0F) {
            m_rollSpeed += 0.005;
        }
        thinkAboutIndividualCreditOffsets();
    }
}

// this will basically compare 2 credits values, and per offset determine whether
// it has to go 'up' or 'down'
void CreditsDrawer::thinkAboutIndividualCreditOffsets()
{
    std::string current_credits = creditsToDigits(m_currentCredits);
    std::string previous_credits = creditsToDigits(m_previousCredits);

    for (int i = 0; i < 6; i++) {
        int currentId = CreditsMath::getCreditDrawId(current_credits[i]);
        int previousId = CreditsMath::getCreditDrawId(previous_credits[i]);

        if (m_offset_direction[i] == 0) {
            if (currentId == CREDITS_NONE || previousId == CREDITS_NONE) {
                // either of the 2 is unidentifiable. Do nothing
                m_offset_direction[i] = 3;
                continue; // next
            }

            // UP/DOWN ANIMATION: Is over the entire sum of money, so all 'credits' move the same
            // way.
            if (m_currentCredits > m_previousCredits) {
                m_offset_direction[i] = 1;
            }
            else if (m_previousCredits > m_currentCredits) {
                m_offset_direction[i] = 2;
            }

            // when the same, do not animate
            if (currentId == previousId) {
                m_offset_direction[i] = 3;
            }

        }

        // always 'add' here, the draw routine knows how to handle this for up/down movement.
        if (m_offset_credit[i] > 18.0F) {
            //m_offset_credit[i] = 18.1F; // so we do not keep matching our IF :)
            m_offset_credit[i] = 0.0F; // so we do not keep matching our IF :)
            if (m_playCreditsSound && m_soundsMade < 7) {
                playSoundWithDistance(m_audioContext, m_mapCamera, m_objects, m_soundType, 32);
                m_soundsMade++;
            }
            // it is fully 'moved'. Now update the array.
            previous_credits[i] = current_credits[i];
            m_previousCredits = std::atoi(previous_credits.c_str());
        }
        else {
            m_offset_credit[i] += 1.0F * m_rollSpeed;
        }
    }
}

/**
 * Create bitmap to draw credits
 */
void CreditsDrawer::draw()
{
    auto *tex = m_gfxinter->getTexture(CREDITS_BAR);
    m_sdlDrawer->renderSprite(tex, m_drawX, m_drawY);
    m_sdlDrawer->setClippingFor(m_drawX+1, m_drawY+5, m_drawX+tex->w-1, m_drawY+tex->h-5);
    drawCurrentCredits(m_drawX, m_drawY);
    drawPreviousCredits(m_drawX, m_drawY);
    m_sdlDrawer->resetClippingFor();

}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawCurrentCredits(int drawX, int drawY)
{
    if (m_currentCredits < 0) m_currentCredits = 0;

    std::string credits = creditsToDigits(m_currentCredits);

    int offset = CreditsMath::getXDrawingOffset(m_currentCredits);

    for (int i=0; i < 6; i++) {
        // the actual position to draw on is: ((game.m_screenX - 120) +
        //        screen        -  14 + (6 digits (each 20 pixels due borders))
        int dx = ((offset + i) * 20);
        int dy = 0;

        // the Y is per credit direction
        if (m_offset_direction[i] == 1) {
            // up, means it comes from 'below' and the m_offset_credit is the 'coming up' part.
            dy = 20 - static_cast<int>(m_offset_credit[i]);
        }
        else if (m_offset_direction[i] == 2) {
            dy = -20 + static_cast<int>(m_offset_credit[i]);
        }

        int nr = CreditsMath::getCreditDrawId(credits[i]);

        if (nr != CREDITS_NONE) {
            m_sdlDrawer->renderSprite(m_gfxdata->getTexture(nr), drawX+dx+8, drawY+dy+8);
        }
    }
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawPreviousCredits(int drawX, int drawY)
{
    if (m_previousCredits < 0) m_previousCredits = 0;

    std::string credits = creditsToDigits(m_previousCredits);

    int offset = CreditsMath::getXDrawingOffset(m_previousCredits);

    for (int i=0; i < 6; i++) {
        // the actual position to draw on is: ((game.m_screenX - 120) +
        //        screen        -  14 + (6 digits (each 20 pixels due borders))
        int dx = ((offset + i) * 20);
        int dy = 0;

        // the Y is per credit direction
        if (m_offset_direction[i] == 1) {
            // up, means it comes from 'below' and the m_offset_credit is the 'coming up' part.
            dy = static_cast<int>(m_offset_credit[i]) * -1;
        }
        else if (m_offset_direction[i] == 2) {
            dy = static_cast<int>(m_offset_credit[i]);
        }
        int nr = CreditsMath::getCreditDrawId(credits[i]);

        if (nr != CREDITS_NONE) {
            m_sdlDrawer->renderSprite(m_gfxdata->getTexture(nr), drawX+dx+8, drawY+dy+8);
        }
    }
}

// the think method will gradually update the m_currentCredits array so that it is 'the same'
bool CreditsDrawer::hasDrawnCurrentCredits()
{
    return m_previousCredits == m_currentCredits;
}

void CreditsDrawer::setPlayer(cPlayer *thePlayer)
{
    this->m_player = thePlayer;
}


// UNCOMMENT THIS IF YOU WANT PER CREDIT AN UP/DOWN ANIMATION, move this piece into
// thinkAboutIndividualCreditOffsets
//
//			if (currentId > previousId) {
//				m_offset_direction[i] = 1; // UP
//			} else if (currentId < previousId) {
//				m_offset_direction[i] = 2; // DOWN
//			}
