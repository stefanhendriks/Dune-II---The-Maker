#include "cHeavyFactory.h"

#include "d2tmc.h"
#include "definitions.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"

cHeavyFactory::cHeavyFactory()
{
    // other variables (class specific)
    drawFlash = false;
    flashes = 0;
    TIMER_animation = 0;
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
        drawFlash = false;
        flashes = 0;
        return; // do nothing when not animating
    }

    if (iFrame < 10) {
        TIMER_animation++;
        if (TIMER_animation > 35) {
            TIMER_animation = 0;
            iFrame++;
        }
    }

    if (flashes > 0) {
        TIMER_flag++;
        if (TIMER_flag > 70) {
            flashes--;
            drawFlash = !drawFlash;
            TIMER_flag = 0;
        }
    }
    else {
        TIMER_flag = 0;
        drawFlash = false;
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
    if (drawFlash) {
        int pixelWidth = getWidthInPixels();
        int pixelHeight = getHeightInPixels();

        // structures are animated within the same source bitmap. The Y coordinates determine
        // what frame is being drawn. So multiply the height of the structure size times frame
        int iSourceY = pixelHeight * 0; // flash have only 1 frame for now

        int drawX = iDrawX();
        int drawY = iDrawY();

        int scaledWidth = mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = mapCamera->factorZoomLevel(pixelHeight);

        Texture *bitmapToDraw = getPlayer()->getStructureBitmapFlash(getType());
        // renderDrawer->maskedStretchBlit(bitmapToDraw, bmp_screen, 0, iSourceY, pixelWidth, pixelHeight, drawX, drawY, scaledWidth, scaledHeight);
        SDL_Rect src = { 0, iSourceY, pixelWidth, pixelHeight};
        SDL_Rect dest= { drawX, drawY, scaledWidth, scaledHeight};
        renderDrawer->renderStrechSprite(bitmapToDraw, src, dest);


    }
}

void cHeavyFactory::startAnimating()
{
    if (isAnimating()) {
        flashes = 5;
        drawFlash = true;
    }
    else {
        iFrame = 0;
        flashes = 0;
        drawFlash = false;
        TIMER_flag = 0;
    }
}