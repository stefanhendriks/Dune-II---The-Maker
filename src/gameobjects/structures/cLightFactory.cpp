#include "cLightFactory.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "definitions.h"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"

// Constructor
cLightFactory::cLightFactory()
{
    // other variables (class specific)
    drawFlash = false;
    flashes = 0;
}

int cLightFactory::getType() const
{
    return LIGHTFACTORY;
}

void cLightFactory::thinkFast()
{
    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();
}

void cLightFactory::think_animation_unitDeploy()
{
    if (!isAnimating()) {
        drawFlash = false;
        flashes = 0;
        return; // do nothing when not animating
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
        setAnimating(false);
    }
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cLightFactory::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_animation_unitDeploy();
}

void cLightFactory::think_guard()
{

}

void cLightFactory::draw()
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

        SDL_Surface *bitmapToDraw = getPlayer()->getStructureBitmapFlash(getType());
        renderDrawer->maskedStretchBlit(bitmapToDraw, bmp_screen, 0, iSourceY, pixelWidth, pixelHeight,
                                        drawX, drawY, scaledWidth, scaledHeight);
    }
}

void cLightFactory::startAnimating()
{
    if (isAnimating()) {
        flashes = 5;
        drawFlash = true;
    }
    else {
        TIMER_flag = 0;
        drawFlash = false;
    }
}