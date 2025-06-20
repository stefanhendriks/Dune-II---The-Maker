#include "cMessageDrawer.h"

#include "data/gfxinter.h"
#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "sidebar/cSideBar.h"

// #include <alfont.h>
#include <allegro.h>
#include <SDL2/SDL.h>

// Game Playing State and SelectYourNextConquestState have their own version
// this class has to be abstracted in such a way so it is used in both those states, yet without the wonky init
// functions

cMessageDrawer::cMessageDrawer()
{
    m_bmpBar = nullptr;
    m_temp = nullptr;
    m_keepMessage = false;
    m_timeMessageIsVisible = 10;
    init();
}

cMessageDrawer::~cMessageDrawer()
{
    init();
}

void cMessageDrawer::destroy()
{
    m_alpha = -1;
    m_TIMER_message = 0;

    destroy_bitmap(m_bmpBar);
    destroy_bitmap(m_temp);
}

void cMessageDrawer::init()
{
    m_state = messages::eMessageDrawerState::COMBAT;
    m_fadeState = messages::eMessageDrawerFadingState::FADE_IN;
    m_keepMessage = false;
    m_alpha = -1;
    m_message.clear();
    m_TIMER_message = 0;
    initCombatPosition();
}

void cMessageDrawer::createMessageBarBmp(int desiredWidth)
{
    if (m_bmpBar) {
        destroy_bitmap(m_bmpBar);
    }

    if (m_temp) {
        destroy_bitmap(m_temp);
    }

    m_bmpBar = create_bitmap(desiredWidth, 30);
    clear_to_color(m_bmpBar, makecol(255, 0, 255));

    renderDrawer->drawSprite(m_bmpBar, (BITMAP *)gfxinter[MESSAGE_LEFT].dat, 0, 0);
    for (int drawX = 11; drawX < m_bmpBar->w; drawX+= 55) {
        renderDrawer->drawSprite(m_bmpBar, (BITMAP *)gfxinter[MESSAGE_MIDDLE].dat, drawX, 0);
    }

    renderDrawer->drawSprite(m_bmpBar, (BITMAP *)gfxinter[MESSAGE_RIGHT].dat, m_bmpBar->w - 11, 0);

    // create this one which we use for actual drawing
    m_temp = create_bitmap(m_bmpBar->w, m_bmpBar->h);
}

void cMessageDrawer::thinkFast()
{
    if (m_fadeState == messages::eMessageDrawerFadingState::FADE_IN) {
        m_TIMER_message++;

        if (m_TIMER_message > m_timeMessageIsVisible) {
            if (!m_keepMessage) {
                m_fadeState = messages::eMessageDrawerFadingState::FADE_OUT;
            }
        }

        m_alpha += 3;
        // fade in, with a max...
        if (m_alpha > 254) {
            m_alpha = 255;
        }
        return;
    }

    // fade out
    m_alpha -= 6;
    if (m_alpha < 0) {
        m_alpha = -1;
    }

    // clear message
    if (m_alpha < 1) {
        m_message.clear();
    }
    m_TIMER_message = 0;
}

/**
 * This sets a message on the message bar, which will fade out
 * after a specific amount of time and then get cleared again.
 *
 * @param msg
 */
void cMessageDrawer::setMessage(const std::string &msg, bool keepMessage)
{
    m_TIMER_message=0;
    m_fadeState = messages::eMessageDrawerFadingState::FADE_IN;
    m_message = msg;
    setKeepMessage(keepMessage);
}

void cMessageDrawer::draw()
{
    if (m_state == messages::eMessageDrawerState::COMBAT) {
        renderDrawer->drawSprite(bmp_screen, m_bmpBar, m_position.x, m_position.y);
    }

    if (m_alpha > -1) {
        set_trans_blender(0, 0, 0, m_alpha);

        clear_to_color(m_temp, makecol(255, 0, 255));

        renderDrawer->drawSprite(m_temp, m_bmpBar, 0, 0);

        // draw message
        renderDrawer->setClippingFor(m_temp, 0, 0, m_bmpBar->w - 10, m_bmpBar->h);
        //Mira TEXT alfont_textprintf(m_temp, game_font, 13, 21, makecol(0, 0, 0), m_message.c_str());
        renderDrawer->resetClippingFor(m_temp);

        // draw temp
        draw_trans_sprite(bmp_screen, m_temp, m_position.x, m_position.y);
    }
}

void cMessageDrawer::initRegionPosition(int offsetX, int offsetY)
{
    m_state = messages::eMessageDrawerState::NEXT_CONQUEST;
    m_keepMessage = false;
    m_timeMessageIsVisible = 650; // sensible default

    int desiredWidth = 480;

    createMessageBarBmp(desiredWidth);

    // default positions region mode
    m_position.x = offsetX + 73;
    m_position.y = offsetY + 358;
}

void cMessageDrawer::initCombatPosition()
{
    m_state = messages::eMessageDrawerState::COMBAT;
    m_keepMessage = false;
    m_timeMessageIsVisible = 450; // sensible default

    int margin = 4;
    int widthOfOptionsButton = 160 + margin;
    int desiredWidth = game.m_screenW - (cSideBar::SidebarWidth + widthOfOptionsButton);
    createMessageBarBmp(desiredWidth);
    // default positions in-game (battle mode)
    m_position.x = widthOfOptionsButton;
//    y = 42;
    m_position.y = 1;
}

void cMessageDrawer::setKeepMessage(bool value)
{
    m_keepMessage = value;
}
