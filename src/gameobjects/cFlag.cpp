#include "cFlag.h"
#include "game/cGameSettings.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"
#include "include/Texture.hpp"
#include <cassert>

cFlag::cFlag(cPlayer *player, cPoint &absCoords, int frames, int animationDelay)
    : m_absCoords(absCoords)
    , m_player(player)
    , m_TIMER_animate(0)
    , m_big(true)
    , m_animationDelay(animationDelay)
    , m_frames(frames)
    , m_frame(0)
{
    assert(player != nullptr);
}

void cFlag::draw()
{
    Texture *flagBitmap = m_big? m_player->getFlagBitmap() : m_player->getFlagSmallBitmap();
    if (!flagBitmap) return;

    int drawX = game.m_mapCamera->getWindowXPosition(m_absCoords.x);
    int drawY = game.m_mapCamera->getWindowYPosition(m_absCoords.y);

    if ((drawX >= 0 && drawX < game.m_gameSettings->getScreenW()) && (drawY >= 0 && drawY < game.m_gameSettings->getScreenH())) { // within screen
        // draw it

        int pixelWidth = flagBitmap->w;
        int pixelHeight = flagBitmap->h / m_frames; // frames in a flag

        // The Y coordinates determine what frame is being drawn.
        // So multiply the height of the flag size times frame
        int iSourceY = pixelHeight * m_frame;

        int scaledWidth = game.m_mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = game.m_mapCamera->factorZoomLevel(pixelHeight);
        cRectangle src = {0, iSourceY, pixelWidth, pixelHeight};
        cRectangle dest = {drawX, drawY, scaledWidth, scaledHeight};

        global_renderDrawer->renderStrechSprite(flagBitmap, src, dest);
    }
}

void cFlag::thinkFast()
{
    m_TIMER_animate++;
    if (m_TIMER_animate > m_animationDelay) {
        m_frame++;
        if (m_frame > (m_frames-1)) {
            m_frame = 0;
        }
        m_TIMER_animate = 0;
    }
}

cFlag *cFlag::createBigFlag(cPlayer *player, cPoint &position)
{
    cPoint correctedPoint = cPoint(
                                position.x,
                                position.y
                            );
    correctedPoint.x -= 15; // width of flag
    cFlag *pFlag = new cFlag(player, correctedPoint, 12, 24);
    pFlag->setBig(true);
    return pFlag;
}

cFlag *cFlag::createSmallFlag(cPlayer *player, cPoint &position)
{
    cPoint correctedPoint = cPoint(
                                position.x,
                                position.y
                            );
    correctedPoint.x -= 10; // width of flag
    cFlag *pFlag = new cFlag(player, correctedPoint, 12, 24);
    pFlag->setBig(false);
    return pFlag;
}
