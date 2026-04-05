#include "cHeavyFactory.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "definitions.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"

cHeavyFactory::cHeavyFactory()
{
    // other variables (class specific)
    m_drawFlash = false;
    m_flashes = 0;
    m_TIMER_animation = 0;
}

int cHeavyFactory::getType() const
{
    return HEAVYFACTORY;
}

void cHeavyFactory::thinkFast()
{
    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();
}

// think animation for unit deployment (when building unit is finished)
void cHeavyFactory::think_animation_unitDeploy()
{
    if (!isAnimating()) {
        m_drawFlash = false;
        m_flashes = 0;
        return; // do nothing when not animating
    }

    if (iFrame < 10) {
        m_TIMER_animation++;
        if (m_TIMER_animation > 35) {
            m_TIMER_animation = 0;
            iFrame++;
        }
    }

    if (m_flashes > 0) {
        TIMER_flag++;
        if (TIMER_flag > 70) {
            m_flashes--;
            m_drawFlash = !m_drawFlash;
            TIMER_flag = 0;
        }
    }
    else {
        TIMER_flag = 0;
        m_drawFlash = false;
    }

    if (iFrame > 9) {
        setAnimating(false);
    }
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cHeavyFactory::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_animation_unitDeploy();
}

void cHeavyFactory::think_guard()
{

}

void cHeavyFactory::draw()
{
    drawWithShadow();
    // draw flashing light
    if (m_drawFlash) {
        int pixelWidth = getWidthInPixels();
        int pixelHeight = getHeightInPixels();

        // structures are animated within the same source bitmap. The Y coordinates determine
        // what frame is being drawn. So multiply the height of the structure size times frame
        int iSourceY = pixelHeight * 0; // flash have only 1 frame for now

        int drawX = iDrawX();
        int drawY = iDrawY();

        int scaledWidth = game.m_mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = game.m_mapCamera->factorZoomLevel(pixelHeight);

        Texture *bitmapToDraw = getPlayer()->getStructureBitmapFlash(getType());
        cRectangle src = { 0, iSourceY, pixelWidth, pixelHeight};
        cRectangle dest= { drawX, drawY, scaledWidth, scaledHeight};
        global_renderDrawer->renderStrechSprite(bitmapToDraw, src, dest);


    }
}

void cHeavyFactory::startAnimating()
{
    if (isAnimating()) {
        m_flashes = 5;
        m_drawFlash = true;
    }
    else {
        iFrame = 0;
        m_flashes = 0;
        m_drawFlash = false;
        TIMER_flag = 0;
    }
}