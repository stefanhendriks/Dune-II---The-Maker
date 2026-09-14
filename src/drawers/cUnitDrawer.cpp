#include "cUnitDrawer.h"

#include "gameobjects/units/cUnit.h"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/players/cPlayer.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "utils/Graphics.hpp"
#include "utils/common.h"
#include "utils/Color.hpp"
#include "utils/cRectangle.h"
#include "utils/d2tm_math.h"
#include "data/gfxdata.h"
#include "include/definitions.h"
#include "include/Facing.h"
#include "include/cAssert.h"

#include <SDL3/SDL.h>
#include <cmath>
#include <format>

cUnitDrawer::cUnitDrawer(cUnit &unit, SDLDrawer *renderer, Graphics *gfxdata) :
    m_unit(unit),
    m_renderer(renderer),
    m_gfxdata(gfxdata)
{
    d2tm_assert(renderer != nullptr);
    d2tm_assert(gfxdata != nullptr);
}

void cUnitDrawer::draw_spice()
{
    float width_x = m_unit.getMapCamera()->factorZoomLevel(m_unit.getBmpWidth());
    int height_y = m_unit.getMapCamera()->factorZoomLevel(4);
    int drawx = m_unit.draw_x();
    int drawy = m_unit.draw_y() - ((height_y * 2) + 2);

    int max = m_unit.getUnitInfo().credit_capacity;
    int w = healthBar(width_x, m_unit.iCredits, max);

    // bar itself
    m_renderer->renderRectFillColor(drawx, drawy, width_x, height_y, 0, 0, 0,ShadowTrans);
    m_renderer->renderRectFillColor(drawx, drawy, w, height_y, 255, 91, 1,ShadowTrans);

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        m_renderer->renderRectColor(drawx, drawy,width_x, height_y, 255, 255, 255,ShadowTrans);
    }
}

void cUnitDrawer::draw_health()
{
    if (m_unit.getHitPoints() < 0) return;

    // draw units health
    float width_x = m_unit.getMapCamera()->factorZoomLevel(m_unit.getBmpWidth());
    int height_y = m_unit.getMapCamera()->factorZoomLevel(4);
    int drawx = m_unit.draw_x();
    int drawy = m_unit.draw_y() - (height_y + 2);

    float healthNormalized = m_unit.getHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

    // bar itself
    m_renderer->renderRectFillColor(drawx, drawy, width_x, height_y, 0, 0, 0,ShadowTrans);
    m_renderer->renderRectFillColor(drawx, drawy, (w - 1), height_y, (Uint8)r,(Uint8)g, 32,ShadowTrans);

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        m_renderer->renderRectColor(drawx, drawy, width_x, height_y, 255, 255, 255,ShadowTrans);
    }
}

void cUnitDrawer::draw_group(cTextDrawer* textDrawer)
{
    if (m_unit.getHitPoints() < 0) return;

    int height_y = m_unit.getMapCamera()->factorZoomLevel(4);
    int drawx = m_unit.draw_x();
    int drawy = m_unit.draw_y() - (height_y + 2);
    // draw group
    // TODO: make text smaller depending on zoom factor?
    if (m_unit.iPlayer == HUMAN) {
        std::string groups;
        for (int i = 0; i < MAX_UNIT_GROUPS; i++) {
            if (m_unit.iGroups[i]) {
                if (!groups.empty()) {
                    groups += " ";
                }
                groups += std::to_string(i + 1);
            }
        }

        if (!groups.empty()) {
            textDrawer->drawText(drawx + 26, drawy - 11, Color::Black, groups);
            textDrawer->drawText(drawx + 26, drawy - 12, Color::White, groups);
        }
    }
}

void cUnitDrawer::draw_experience()
{
    int iStars = (int) m_unit.fExperience;

    if (iStars < 1)
        return; // no stars to draw!

    int iStarType = 0;

    // twice correct and upgrade star type
    if (iStars > 2) {
        iStarType++;
        iStars -= 3;
    }

    // red stars now, very much experience!
    if (iStars > 2) {
        iStarType++;
        iStars -= 3;
    }

    // still enough experience! wow
    if (iStars > 2) {
        iStars = 3;
    }


    int drawx = m_unit.draw_x() + 3;
    int drawy = m_unit.draw_y() - 19;
    // 1 star = 1 experience
    for (int i = 0; i < iStars; i++) {
        m_renderer->renderSprite(m_gfxdata->getTexture(OBJECT_STAR_01 + iStarType), drawx + i * 9, drawy, ShadowTrans);
    }
}

void cUnitDrawer::draw_path() const
{
    // for debugging purposes
    if (m_unit.movement.iGoalCell == m_unit.getCell())
        return;

    if (m_unit.movement.iPath[0] < 0)
        return;

    int halfTile = 16;
    int iPrevX = m_unit.getMapCamera()->getWindowXPositionFromCellWithOffset(m_unit.movement.iPath[0], halfTile);
    int iPrevY = m_unit.getMapCamera()->getWindowYPositionFromCellWithOffset(m_unit.movement.iPath[0], halfTile);

    for (int i = 1; i < MAX_PATH_SIZE; i++) {
        if (m_unit.movement.iPath[i] < 0) break;
        int iDx = m_unit.getMapCamera()->getWindowXPositionFromCellWithOffset(m_unit.movement.iPath[i], halfTile);
        int iDy = m_unit.getMapCamera()->getWindowYPositionFromCellWithOffset(m_unit.movement.iPath[i], halfTile);

        if (i == m_unit.movement.iPathIndex) { // current node we navigate to
            m_renderer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 255, 255,255});
        }
        else if (m_unit.movement.iPath[i] == m_unit.movement.iGoalCell) {
            // end of path (goal)
            m_renderer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 0, 0,255});
        }
        else {
            // everything else
            m_renderer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 255, 64,255});
        }

        // draw a line from previous to current
        iPrevX = iDx;
        iPrevY = iDy;
    }

    // Render remembered waypoint cells as blue dots.
    for (int i = 0; i < MAX_WAYPOINTS_SIZE; i++) {
        int waypointCell = m_unit.movement.waypointCells[i];
        if (waypointCell < 0) {
            break;
        }

        int waypointX = m_unit.getMapCamera()->getWindowXPositionFromCellWithOffset(waypointCell, halfTile);
        int waypointY = m_unit.getMapCamera()->getWindowYPositionFromCellWithOffset(waypointCell, halfTile);
        m_renderer->renderDot(waypointX-2, waypointY-2, Color{64, 160, 255, 192}, 4);
    }
}

void cUnitDrawer::draw()
{
    if (m_unit.isHidden()) {
        // temp hitpoints filled, meaning it is not visible (but not dead). Ie, it is being repaired, or transfered
        // by carry-all
        return;
    }

    // Selection box x, y position. Depends on unit size
    const int ux = m_unit.draw_x();
    const int uy = m_unit.draw_y();

    if (m_unit.isSandworm()) {
        return;
    }

    s_UnitInfo &unitType = m_unit.getUnitInfo();
    const int bmp_width = unitType.bmp_width;
    const int bmp_height = unitType.bmp_height;

    // the multiplier we will use to draw the unit
    const int bmp_head = convertAngleToDrawIndex(facingToInt(m_unit.rendering.iHeadFacing));
    const int bmp_body = convertAngleToDrawIndex(facingToInt(m_unit.rendering.iBodyFacing));

    // draw body first
    int start_x = bmp_body * bmp_width;
    int start_y = bmp_height * m_unit.rendering.iFrame;

    cPlayer *cPlayer = m_unit.getPlayer();

    const float scaledWidth = m_unit.getMapCamera()->factorZoomLevel(bmp_width);
    const float scaledHeight = m_unit.getMapCamera()->factorZoomLevel(bmp_height);

    Texture *shadow = cPlayer->getUnitShadowBitmap(m_unit.iType);
    int roundedScaledWidth = static_cast<int>(round(scaledWidth));
    int roundedScaledHeight = static_cast<int>(round(scaledHeight));
    if (shadow) {
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, roundedScaledWidth, roundedScaledHeight};
        if (m_unit.iType == CARRYALL) {
            dest = {ux, uy+24, roundedScaledWidth, roundedScaledHeight};
        }
        m_renderer->renderStrechSprite(shadow,src, dest, ShadowTrans);
    }

    // Draw BODY
    Texture *bitmap = cPlayer->getUnitBitmap(m_unit.iType);
    if (bitmap) {
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, roundedScaledWidth, roundedScaledHeight};
        m_renderer->renderStrechSprite(bitmap,src, dest);
    }
    else {
        m_unit.log(std::format("unit of iType [{}] did not have a bitmap!?", m_unit.iType));
    }


    // Draw TOP
    Texture *top = cPlayer->getUnitTopBitmap(m_unit.iType);
    if (top && m_unit.getHitPoints() > -1) {
        // recalculate start_x using head instead of body
        start_x = bmp_head * bmp_width;
        start_y = bmp_height * m_unit.rendering.iFrame;
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, static_cast<int>(round(m_unit.getMapCamera()->factorZoomLevel(bmp_width))), static_cast<int>(round(m_unit.getMapCamera()->factorZoomLevel(bmp_height)))};
        m_renderer->renderStrechSprite(top,src, dest);
    }

    // when we want to be picked up..
    if (m_unit.bCarryMe) {
        m_renderer->renderSprite(m_gfxdata->getTexture(SYMB_PICKMEUP), ux, uy - 7);
    }

    if (m_unit.isSelected()) {
        SDL_Surface *focusBitmap = m_gfxdata->getSurface(FOCUS);
        int bmp_width = focusBitmap->w;
        int bmp_height = focusBitmap->h;

        int x = m_unit.draw_x(bmp_width);
        int y = m_unit.draw_y(bmp_height);

        cRectangle dest = {x,y, static_cast<int>(round(m_unit.getMapCamera()->factorZoomLevel(bmp_width))),static_cast<int>(round(m_unit.getMapCamera()->factorZoomLevel(bmp_height)))};
        m_renderer->renderStrechFullSprite(m_gfxdata->getTexture(FOCUS), dest);
    }

    if (m_unit.isDrawUnitDebugEnabled()) {
        // render pixel at the very center
        m_renderer->renderDot(m_unit.center_draw_x(), m_unit.center_draw_y(), Color{255, 255, 0,255},2);

        // render from the units top-left to center pixel
        m_renderer->renderLine( m_unit.draw_x(), m_unit.draw_y(), m_unit.center_draw_x(), m_unit.center_draw_y(), Color{255, 255, 0,255});
    }
}

void cUnitDrawer::draw_debug(cTextDrawer* textDrawer)
{
    const cRectangle &dimensions = m_unit.getDimensions();
    m_renderer->renderRectColor(dimensions.getX(),dimensions.getY(), dimensions.getWidth(),dimensions.getHeight(), Color{255, 0, 255,ShadowTrans});
    m_renderer->renderDot(m_unit.center_draw_x(), m_unit.center_draw_y(), Color{255, 0, 255,ShadowTrans},1);
    textDrawer->drawText(m_unit.draw_x(), m_unit.draw_y(), Color{255, 255, 255,ShadowTrans}, std::format("{}", m_unit.iID));
    if (m_unit.isSandworm()) {
        textDrawer->drawText(m_unit.draw_x(), m_unit.draw_y()-16, Color{255,255,255,255}, std::format("{} / {} / {}", m_unit.getUnitsEaten(), m_unit.guardTimer.get(), m_unit.movewaitTimer.get()));
    }
}
