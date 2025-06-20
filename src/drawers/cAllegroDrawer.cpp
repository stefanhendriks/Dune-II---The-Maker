#include "cAllegroDrawer.h"

#include "d2tmc.h"

#include <allegro.h>

#include <algorithm>
#include <memory>

cAllegroDrawer::cAllegroDrawer(cAllegroDataRepository *dataRepository) : m_dataRepository(dataRepository)
{
    colorBlack=makecol(0,0,0);
    gui_colorWindow = makecol(176,176,196);
    gui_colorBorderDark = makecol(84,84,120);
    gui_colorBorderLight = makecol(252,252,252);
}

cAllegroDrawer::~cAllegroDrawer()
{
    // do not delete data repository, we do not own it!
    m_dataRepository = nullptr;

    for (auto &p : bitmapCache) {
        destroy_bitmap(p.second);
    }
}

void cAllegroDrawer::stretchSprite(BITMAP *src, BITMAP *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
{
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    stretch_sprite(dest, src, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::stretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
{
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width < 0) return;
    if (height < 0) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::stretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
{
    stretchBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::maskedBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height)
{
    if (src == nullptr) return;
    if (dest == nullptr) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width <= 0) return;
    if (height <= 0) return;

    // no use drawing outside of bounds
    if (pos_x + width < 0) return;
    if (pos_y + height < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    masked_blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void cAllegroDrawer::maskedBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height)
{
    maskedBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void cAllegroDrawer::maskedStretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
{
    maskedStretchBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::maskedStretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
{
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width <= 0) return;
    if (height <= 0) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    masked_stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::drawCenteredSprite(BITMAP *dest, BITMAP *src)
{
    assert(src);
    assert(dest);
    int xPos = getCenteredXPosForBitmap(src, game.m_screenW);
    int yPos = getCenteredYPosForBitmap(src);
    draw_sprite(dest, src, xPos, yPos);
}

void cAllegroDrawer::drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP *src, float percentage)
{
    int xPos = getCenteredXPosForBitmap(src, game.m_screenW);

    // we want to know the 'center' first. This is done in the percentage
    int wantedYPos = ((float)game.m_screenH * percentage);

    // we need to know the height of the src
    int height = src->h;
    int halfOfHeight = height / 2;
    int yPos = wantedYPos - halfOfHeight;
    draw_sprite(dest, src, xPos, yPos);
}


void cAllegroDrawer::drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y, int totalWidth, int xOffset)
{
    assert(src);
    assert(dest);
    int xPos = getCenteredXPosForBitmap(src, totalWidth) + xOffset;
    draw_sprite(dest, src, xPos, y);
}

void cAllegroDrawer::drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x)
{
    assert(src);
    assert(dest);
    int yPos = getCenteredXPosForBitmap(src, game.m_screenW);
    draw_sprite(dest, src, x, yPos);
}

int cAllegroDrawer::getCenteredXPosForBitmap(BITMAP *bmp, int totalWidth)
{
    assert(bmp);
    int width = bmp->w;
    int halfOfWidth = width / 2;
    return (totalWidth / 2) - halfOfWidth;
}

int cAllegroDrawer::getCenteredYPosForBitmap(BITMAP *bmp)
{
    assert(bmp);
    int height = bmp->h;
    int halfOfHeight = height / 2;
    return (game.m_screenH / 2) - halfOfHeight;
}

// void cAllegroDrawer::blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
//     if (src == nullptr) return;
//     blit(src, dest, src_x, src_y, width, height, pos_x, pos_y);
// }

void cAllegroDrawer::blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) const
{
    // use :: so we use global scope Allegro blitSprite
    if (src == nullptr || dest == nullptr) return;
    ::blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void cAllegroDrawer::blitSprite(BITMAP *src, BITMAP *dest, const cRectangle *rectangle) const
{
    if (rectangle == nullptr) return;
    if (src == nullptr) return;
    if (dest == nullptr) return;
    blit(src, dest, 0, 0, rectangle->getWidth(), rectangle->getHeight(), rectangle->getX(), rectangle->getY());
}

void cAllegroDrawer::drawRect(BITMAP *dest, const cRectangle &pRectangle, int color)
{
    rect(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getEndX(), pRectangle.getEndY(), color);
}

void cAllegroDrawer::drawRect(BITMAP *dest, int x, int y, int width, int height, int color)
{
    if (dest == nullptr) return;
    // the -1 is because the width /height is "including" the current pixel
    // ie, a width of 1 pixel means it draws just 1 pixel, since the x2 is a dest it should result into x1
    rect(dest, x, y, x + (width-1), y + (height-1), color);
}

void cAllegroDrawer::drawRectFilled(BITMAP *dest, const cRectangle &pRectangle, int color)
{
    rectfill(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getEndX(), pRectangle.getEndY(), color);
}

void cAllegroDrawer::drawRectFilled(BITMAP *dest, int x, int y, int width, int height, int color)
{
    rectfill(dest, x, y, x+width, y+height, color);
}

void cAllegroDrawer::drawRectTransparentFilled(BITMAP *dest, const cRectangle &rect, int color, int alpha)
{
    assert(alpha >= 0);
    assert(alpha <= 255);

    auto bitmap = bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}];
    if (bitmap == nullptr) {
        bitmap = create_bitmap(rect.getWidth(), rect.getHeight());
        bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}] = bitmap;
    }

    rectfill(bitmap, 0, 0, rect.getWidth(), rect.getHeight(), color);

    set_trans_blender(0, 0, 0, alpha);
    draw_trans_sprite(dest, bitmap, rect.getX(),rect.getY());
}

// cRectangle *cAllegroDrawer::fromBitmap(int x, int y, BITMAP *src) {
//     return new cRectangle(x, y, src->w, src->h);
// }

void cAllegroDrawer::setTransBlender(int red, int green, int blue, int alpha)
{
    set_trans_blender(std::clamp(red, 0, 255),
                      std::clamp(green, 0, 255),
                      std::clamp(blue, 0, 255),
                      std::clamp(alpha, 0, 255));
}

void cAllegroDrawer::drawSprite(BITMAP *dest, BITMAP *src, int x, int y)
{
    draw_sprite(dest, src, x, y);
}


/**
	Function that will go through all pixels and will replace a certain color with another.
    Ie, this can be used to create the fading animation for Windtraps.
**/
void cAllegroDrawer::bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor)
{
    for (int x = 0; x < bmp->w; x++) {
        for (int y = 0; y < bmp->h; y++) {
            if (getpixel(bmp, x, y) == colorToReplace) {
                putpixel(bmp, x, y, newColor);
            }
        }
    }
}

void cAllegroDrawer::drawSprite(BITMAP *dest, int index, int x, int y)
{
    BITMAP *sBitmap = m_dataRepository->getBitmapAt(index);
    if (!sBitmap) return; // failed, bail!
    drawSprite(dest, sBitmap, x, y);
}

void cAllegroDrawer::resetClippingFor(BITMAP *bmp)
{
    if (!bmp) return;
    setClippingFor(bmp, 0, 0, bmp->w, bmp->h);
}

void cAllegroDrawer::setClippingFor(BITMAP *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY)
{
    if (!bmp) return;
    set_clip_rect(bmp, topLeftX, topLeftY, bottomRightX, bottomRightY);
}

void cAllegroDrawer::blitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y)
{
    BITMAP *src = (BITMAP *) gfxdata[index].dat;
    this->blit(src, dest, src_x, src_y, width, height, pos_x, pos_y);
}

int cAllegroDrawer::getColorByNormValue(int r, int g, int b, float norm)
{
    return makecol((r * norm), (g * norm), (b * norm));
}

void cAllegroDrawer::gui_DrawRect(BITMAP *dest, const cRectangle &rectangle)
{
    gui_DrawRect(dest, rectangle, gui_colorWindow, gui_colorBorderLight, gui_colorBorderDark);
}

void cAllegroDrawer::gui_DrawRect(BITMAP *dest, const cRectangle &rectangle, int gui_colorWindow, int gui_colorBorderLight, int gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();

    drawRectFilled(dest, rectangle, gui_colorWindow);
    drawRect(dest, rectangle, gui_colorBorderLight);

    // fill it up
//    _rectfill(bmp_screen, x1, y1, x1+width, y1+height, makecol(176,176,196));

    // rect
//    rect(bmp_screen, x1,y1,x1+width, y1+height, makecol(252,252,252));

    // lines to darken the right sides
    line(bmp_screen, x1+width, y1, x1+width, y1+height, gui_colorBorderDark);
    line(bmp_screen, x1, y1+height, x1+width, y1+height, gui_colorBorderDark);
}

void cAllegroDrawer::gui_DrawRectBorder(BITMAP *dest, const cRectangle &rectangle, int gui_colorBorderLight,
                                        int gui_colorBorderDark)
{

    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();

    drawRect(dest, rectangle, gui_colorBorderLight);
    line(bmp_screen, x1+width, y1, x1+width, y1+height, gui_colorBorderDark);
    line(bmp_screen, x1, y1+height, x1+width, y1+height, gui_colorBorderDark);
}

void cAllegroDrawer::drawTransSprite(BITMAP *sprite, BITMAP *dest, int x, int y)
{
    draw_trans_sprite(dest, sprite, x, y);
}

void cAllegroDrawer::drawLine(BITMAP *bmp, int x1, int y1, int x2, int y2, int color)
{
    line(bmp, x1, y1, x2, y2, color);
}

void cAllegroDrawer::drawDot(BITMAP *bmp, int x, int y, int color, int size)
{
    if (size < 1) return;

    if (size == 1) {
        putpixel(bmp, x, y, color);
        return;
    }

    int endX = x + size;
    int endY = y + size;
    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            putpixel(bmp, sx, sy, color);
        }
    }
}

void cAllegroDrawer::shimmer(int r, int x, int y, float cameraZoom)
{
    int x1, y1;
    int nx, ny;
    int gp = 0;     // Get Pixel Result
    int tc = 0;

    // go around 360 fDegrees (twice as fast now)
    float step = std::fmax(1.0f, cameraZoom);

    for (float dr = 0; dr < r; dr += step) {
        for (double d = 0; d < 360; d++) {
            x1 = (x + (cos(d) * (dr)));
            y1 = (y + (sin(d) * (dr)));

            if (x1 < 0) x1 = 0;
            if (y1 < 0) y1 = 0;
            if (x1 >= game.m_screenW) x1 = game.m_screenW - 1;
            if (y1 >= game.m_screenH) y1 = game.m_screenH - 1;

            gp = getpixel(bmp_screen, x1, y1); //ALLEGRO // use this inline function to speed up things.
            // Now choose random spot to 'switch' with.
            nx = (x1 - 1) + rnd(2);
            ny = (y1 - 1) + rnd(2);

            if (nx < 0) nx = 0;
            if (ny < 0) ny = 0;
            if (nx >= game.m_screenW) nx = game.m_screenW - 1;
            if (ny >= game.m_screenH) ny = game.m_screenH - 1;

            tc = getpixel(bmp_screen, nx, ny); //ALLEGRO

            if (gp > -1 && tc > -1) {
                // Now switch colors
                putpixel(bmp_screen, nx, ny, gp);
                putpixel(bmp_screen, x1, y1, tc);
            }
        }
    }
}