#include "CreditsDrawer.h"
#include "game/cGameSettings.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/cSoundPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "data/gfxaudio.h"

#include <SDL2/SDL.h>
#include <cassert>

CreditsDrawer::CreditsDrawer(GameContext* ctx, cPlayer *player) :
    m_player(player),
    m_ctx(ctx),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get()),
    m_gfxdata(ctx->getGraphicsContext()->gfxdata.get()),
    m_renderDrawer(ctx->getSDLDrawer())
{
    assert(player!= nullptr);
    assert(ctx != nullptr);

    memset(m_offset_credit, 0, sizeof(m_offset_credit));
    memset(m_offset_direction, 0, sizeof(m_offset_direction));
    m_initial = true;
    m_soundType = -1;
    m_rollSpeed = 0.0F;
    m_rolled = 0;
    m_soundsMade = 0;
    m_currentCredits = 0;
    m_previousCredits = 0;

    // center credits bar within sidebar
    int widthCreditsBar = (m_gfxinter->getSurface(CREDITS_BAR))->w;
    m_drawX = game.m_gameSettings->getScreenW() - (cSideBar::SidebarWidthWithoutCandyBar / 2) - (widthCreditsBar / 2);
    m_drawY = 0;
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
    memset(m_offset_credit, 0, sizeof(m_offset_credit));
    memset(m_offset_direction, 0, sizeof(m_offset_direction));
}

// timer based method
void CreditsDrawer::thinkFast()
{

    if (hasDrawnCurrentCredits() || m_initial) {
        m_soundsMade = 0;

        memset(m_offset_credit, 0, sizeof(m_offset_credit));
        memset(m_offset_direction, 0, sizeof(m_offset_direction));

        // determine new m_currentCredits
        // TODO: make it 'roll' instead of 'jump' to the newest credits?
        m_previousCredits = m_currentCredits;
        auto newCurrentCredits = m_player->getCredits();

        if (newCurrentCredits != m_previousCredits) {
            int diff = newCurrentCredits - m_previousCredits;
            // when big difference, slice it up in pieces so we
            // have a more fluent transition
            if (diff > 200) {
                m_currentCredits = m_previousCredits + (diff / 8);
            }
            else if (diff > 20) {
                m_currentCredits = m_previousCredits + (diff / 4);
            }
            else {
                m_currentCredits = newCurrentCredits;
            }
        }

        // decide what sound to play when done
        if (m_currentCredits > m_previousCredits) {
            m_soundType = SOUND_CREDITUP;
        }
        else if (m_currentCredits < m_previousCredits) {
            m_soundType = SOUND_CREDITDOWN;
        }
        else {
            m_soundType = -1;
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
    char current_credits[9];
    char previous_credits[9];
    memset(current_credits, 0, sizeof(current_credits));
    memset(previous_credits, 0, sizeof(previous_credits));
    snprintf(current_credits, sizeof(current_credits), "%d", m_currentCredits);
    snprintf(previous_credits, sizeof(previous_credits), "%d", m_previousCredits);

    for (int i = 0; i < 6; i++) {
        int currentId = getCreditDrawId(current_credits[i]);
        int previousId = getCreditDrawId(previous_credits[i]);

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
            if (m_soundsMade < 7) {
                game.playSound(m_soundType, 32);
                m_soundsMade++;
            }
            // it is fully 'moved'. Now update the array.
            previous_credits[i] = current_credits[i];
            m_previousCredits = atoi(previous_credits);
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
    m_renderDrawer->renderSprite(tex, m_drawX, m_drawY);
    m_renderDrawer->setClippingFor(m_drawX+1, m_drawY+5, m_drawX+tex->w-1, m_drawY+tex->h-5);
    drawCurrentCredits(m_drawX, m_drawY);
    drawPreviousCredits(m_drawX, m_drawY);
    m_renderDrawer->resetClippingFor();

}

int CreditsDrawer::getXDrawingOffset(int amount)
{
    int offset = 1;
    if (amount > 0)		offset =  4;
    if (amount < 10)	offset =  5;
    if (amount > 99)	offset =  3;
    if (amount > 999)	offset =  2;
    if (amount > 9999) 	offset =  1;
    if (amount > 99999) offset =  0;
    return offset;
}

int CreditsDrawer::getCreditDrawId(char c)
{
    if (c == '0') return CREDITS_0;
    if (c == '1') return CREDITS_1;
    if (c == '2') return CREDITS_2;
    if (c == '3') return CREDITS_3;
    if (c == '4') return CREDITS_4;
    if (c == '5') return CREDITS_5;
    if (c == '6') return CREDITS_6;
    if (c == '7') return CREDITS_7;
    if (c == '8') return CREDITS_8;
    if (c == '9') return CREDITS_9;
    return CREDITS_NONE;
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawCurrentCredits(int drawX, int drawY)
{
    if (m_currentCredits < 0) m_currentCredits = 0;

    char credits[9];
    memset(credits, 0, sizeof(credits));
    snprintf(credits, sizeof(credits), "%d", m_currentCredits);

    int offset = getXDrawingOffset(m_currentCredits);

    for (int i=0; i < 6; i++) {
        // the actual position to draw on is: ((game.m_screenX - 120) +
        //        screen        -  14 + (6 digits (each 20 pixels due borders))
        int dx = ((offset + i) * 20);
        int dy = 0;

        // the Y is per credit direction
        if (m_offset_direction[i] == 1) {
            // up, means it comes from 'below' and the m_offset_credit is the 'coming up' part.
            dy = 20 - (int)m_offset_credit[i];
        }
        else if (m_offset_direction[i] == 2) {
            dy = -20 + (int)m_offset_credit[i];
        }

        int nr = getCreditDrawId(credits[i]);

        if (nr != CREDITS_NONE) {
            m_renderDrawer->renderSprite(m_gfxdata->getTexture(nr), drawX+dx+8, drawY+dy+8);
        }
    }
}

// current credits are either drawn from 'above' or from below. (depending on direction)
// the offset pushes them more up or down.
void CreditsDrawer::drawPreviousCredits(int drawX, int drawY)
{
    if (m_previousCredits < 0) m_previousCredits = 0;

    char credits[9];
    memset(credits, 0, sizeof(credits));
    snprintf(credits, sizeof(credits), "%d", m_previousCredits);

    int offset = getXDrawingOffset(m_previousCredits);

    for (int i=0; i < 6; i++) {
        // the actual position to draw on is: ((game.m_screenX - 120) +
        //        screen        -  14 + (6 digits (each 20 pixels due borders))
        int dx = ((offset + i) * 20);
        int dy = 0;

        // the Y is per credit direction
        if (m_offset_direction[i] == 1) {
            // up, means it comes from 'below' and the m_offset_credit is the 'coming up' part.
            dy = (int)m_offset_credit[i] * -1;
        }
        else if (m_offset_direction[i] == 2) {
            dy = (int)m_offset_credit[i];
        }
        int nr = getCreditDrawId(credits[i]);

        if (nr != CREDITS_NONE) {
            m_renderDrawer->renderSprite(m_gfxdata->getTexture(nr), drawX+dx+8, drawY+dy+8);
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
