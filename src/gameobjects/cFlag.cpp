#include "cFlag.h"
#include "game/cGameSettings.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/players/cPlayer.h"
#include "include/Texture.hpp"
#include "include/cAssert.h"

cFlag::cFlag(cPlayer *player, cPoint &absCoords, int frames, int animationDelay, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer)
    : m_absCoords(absCoords)
    , m_player(player)
    , m_mapCamera(mapCamera)
    , m_settings(settings)
    , m_renderer(renderer)
    , m_TIMER_animate(0)
    , m_big(true)
    , m_animationDelay(animationDelay)
    , m_frames(frames)
    , m_frame(0)
{
    d2tm_assert(player != nullptr);
    d2tm_assert(mapCamera != nullptr);
    d2tm_assert(settings != nullptr);
    d2tm_assert(m_renderer != nullptr);
}

void cFlag::draw()
{
    Texture *flagBitmap = m_big? m_player->getFlagBitmap() : m_player->getFlagSmallBitmap();
    if (!flagBitmap) return;

    int drawX = m_mapCamera->getWindowXPosition(m_absCoords.x);
    int drawY = m_mapCamera->getWindowYPosition(m_absCoords.y);

    if ((drawX >= 0 && drawX < m_settings->getScreenW()) && (drawY >= 0 && drawY < m_settings->getScreenH())) {
        int pixelWidth = flagBitmap->w;
        int pixelHeight = flagBitmap->h / m_frames; // frames in a flag

        // The Y coordinates determine what frame is being drawn.
        // So multiply the height of the flag size times frame
        int iSourceY = pixelHeight * m_frame;

        int scaledWidth = m_mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = m_mapCamera->factorZoomLevel(pixelHeight);
        cRectangle src = {0, iSourceY, pixelWidth, pixelHeight};
        cRectangle dest = {drawX, drawY, scaledWidth, scaledHeight};

        m_renderer->renderStrechSprite(flagBitmap, src, dest);
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

std::unique_ptr<cFlag> cFlag::createBigFlag(cPlayer *player, cPoint &position, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer)
{
    cPoint correctedPoint = cPoint(position.x, position.y);
    correctedPoint.x -= 15; // width of flag
    auto pFlag = std::make_unique<cFlag>(player, correctedPoint, 12, 24, mapCamera, settings, renderer);
    pFlag->setBig(true);
    return pFlag;
}

std::unique_ptr<cFlag> cFlag::createSmallFlag(cPlayer *player, cPoint &position, cMapCamera *mapCamera, cGameSettings *settings, SDLDrawer *renderer)
{
    cPoint correctedPoint = cPoint(position.x, position.y);
    correctedPoint.x -= 10; // width of flag
    auto pFlag = std::make_unique<cFlag>(player, correctedPoint, 12, 24, mapCamera, settings, renderer);
    pFlag->setBig(false);
    return pFlag;
}
