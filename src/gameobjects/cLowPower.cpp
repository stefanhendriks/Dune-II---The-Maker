#include "gameobjects/cLowPower.h"

#include "include/d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"
#include "include/Texture.hpp"
#include "utils/cBounceInt.h"

#include <iostream>

cLowPower::cLowPower(cPlayer *player, cPoint &absCoords)
    : player(player), absCoords(absCoords),  intensity(std::make_unique<cBounceInt>(64,512))
{
}

void cLowPower::draw()
{
    Texture *flagBitmap = player->getLowPowerBitmap();
    if (!flagBitmap) return;

    int drawX = mapCamera->getWindowXPosition(absCoords.x);
    int drawY = mapCamera->getWindowYPosition(absCoords.y);
    //std::cout << "lowPower " << drawX << " " << drawY << std::endl;

    if ((drawX >= 0 && drawX < game.m_screenW) && (drawY >= 0 && drawY < game.m_screenH)) { // within screen
        // draw it

        int pixelWidth = flagBitmap->w;
        int pixelHeight = flagBitmap->h;

        int scaledWidth = mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = mapCamera->factorZoomLevel(pixelHeight);
        cRectangle src = {0, 0, pixelWidth, pixelHeight};
        cRectangle dest = {drawX, drawY, scaledWidth, scaledHeight};
        //std::cout << "lowPower " << drawX << " " << drawY << " " << Uint8(8*intensity->get()) << std::endl;
        renderDrawer->renderStrechSprite(flagBitmap, src, dest, Uint8(intensity->get()/2));
    }
}

void cLowPower::thinkFast()
{
    //std::cout << "lowpower update" << std::endl; 
    intensity->update();
}

cLowPower *cLowPower::createFlag(cPlayer *player, cPoint &position)
{
    cPoint correctedPoint = cPoint(
                                position.x,
                                position.y
                            );
    //correctedPoint.x -= 8; // width/2 of flag
    cLowPower *pFlag = new cLowPower(player, correctedPoint);
    return pFlag;
}

