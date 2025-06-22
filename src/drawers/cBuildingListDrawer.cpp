#include "cBuildingListDrawer.h"

#include "drawers/SDLDrawer.hpp"
#include "d2tmc.h"
#include "data/gfxinter.h"
#include "player/cPlayer.h"

#include <SDL2/SDL.h>
#include <fmt/core.h>

cBuildingListDrawer::cBuildingListDrawer(cPlayer *player) :
    m_textDrawer(game_font),
    m_player(player),
    m_renderListIds(false)
{
}

void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw)
{
    if (listIDToDraw == eListTypeAsInt(eListType::LIST_CONSTYARD)) {
        drawListWithStructures(list);
    }
    else {
        drawListWithUnitsOrAbilities(list);
    }
}

void cBuildingListDrawer::drawListWithStructures(cBuildingList *list)
{
    drawList(list, true);
}

void cBuildingListDrawer::drawListWithUnitsOrAbilities(cBuildingList *list)
{
    drawList(list, false);
}

void cBuildingListDrawer::drawButtonHoverRectangle(cBuildingList *list)
{
    assert(list != nullptr);

    int x = list->getButtonDrawX();
    int y = list->getButtonDrawY();
    int id = list->getButtonIconIdUnpressed();

    int width = 33;
    int height = (gfxinter->getSurface(id))->h;

    SDL_Color color = m_player->getSelectFadingColor();

    renderDrawer->drawRect(bmp_screen, x, y, width, height, color);
    renderDrawer->drawRect(bmp_screen, x + 1, y + 1, width-2, height-2, color);

}

void cBuildingListDrawer::drawButton(cBuildingList *list, bool pressed)
{
    assert(list != nullptr);

    int x = list->getButtonDrawX();
    int y = list->getButtonDrawY();
    int id = list->getButtonIconIdUnpressed();

    if (pressed) {
        id = list->getButtonIconIdPressed();
    }

    if (id < 0) {
        // this is not good
        logbook("ERROR UNABLE TO DRAW LIST BECAUSE ID IS NOT VALID!?");
        return;
    }

//	int width = (gfxinter->getSurface(id))->w;
    int width = 33;
    int height = (gfxinter->getSurface(id))->h;
//	_rectfill(bmp_screen, x, y, x+width, y+height, SDL_Color{255, list->getType() * (255/LIST_MAX), list->getType() * (255/LIST_MAX)));

    // clear
    renderDrawer->drawSprite(bmp_screen, gfxinter->getSurface(list->getButtonIconIdUnpressed()), x, y);		// draw pressed button version (unpressed == default in gui)

    // set blender
    // @Mira fix trasnparency set_trans_blender(0,0,0,128);
    renderDrawer->drawSprite(bmp_screen, gfxinter->getSurface(id), x, y);

    if (!list->isAvailable()) {
        renderDrawer->drawRectTransparentFilled(bmp_screen, {x, y, width, height}, SDL_Color{0,0,0,96});
    }

    if (pressed) {
        list->stopFlashing();

        SDL_Color color = m_player->getHouseFadingColor();

        renderDrawer->drawRect(bmp_screen, x, y, width, height, color);
        renderDrawer->drawRect(bmp_screen, x + 1, y + 1, width - 2, height - 2, color);
    }
    else {
        if (list->isFlashing()) {
            SDL_Color color = list->getFlashingColor();

            renderDrawer->drawRect(bmp_screen, x, y, width, height, color);
            renderDrawer->drawRect(bmp_screen, x + 1, y + 1, width - 2, height - 2, color);
            renderDrawer->drawRect(bmp_screen, x + 2, y + 2, width - 3, height - 3, color);
        }
    }
}


int cBuildingListDrawer::getDrawX()
{
    return (game.m_screenW - cSideBar::SidebarWidthWithoutCandyBar) + 2;
}

int cBuildingListDrawer::getDrawY()
{
    return cSideBar::TopBarHeight + 230 + 30;
}

/**
 * Draw the list, start from startId, until the max icons in the list to be drawn.
 *
 * @param startId
 */
void cBuildingListDrawer::drawList(cBuildingList *list, bool shouldDrawStructureSize)
{
    // starting draw coordinates
    int iDrawX=getDrawX();
    int iDrawY=getDrawY();

    SDL_Color selectFadingColor = m_player->getSelectFadingColor();

    int end = MAX_ITEMS;

    // is building an item in the list?
    std::array<int, 5> isBuildingItemInList = list->isBuildingItem();

    // Start drawing the 'icons grid'
    int withOfIcon = 63;
    int heightOfIcon = 47;

    // draw the icons, in rows of 3
    int rowNr = 0;
    for (int i = 0; i < end; i++) {
        cBuildingListItem *item = list->getItem(i);

        if (item == nullptr) {
            continue; // allow gaps in the list data structure (just not with rendering)
        }

        int iDrawXEnd = iDrawX + withOfIcon;
        int iDrawYEnd = iDrawY + heightOfIcon;

        // asumes drawing for human player
        bool cannotPayIt = !m_player->hasEnoughCreditsFor(item->getBuildCost());

        // icon id must be set , assert it.
        assert(item->getIconId() > -1);

        renderDrawer->drawSprite(bmp_screen, gfxinter->getSurface(item->getIconId()), iDrawX, iDrawY);

        if (shouldDrawStructureSize) {
            drawStructureSize(item->getBuildId(), iDrawX, iDrawY);
        }

        // when this item is being built.
        if (item->isBuilding()) {
            int iFrame = item->getBuildProgressFrame();

            if (!item->isDoneBuilding() || iFrame < 31) {
                // draw the other progress stuff
                //@Mira fix transparency // @Mira fix trasnparency set_trans_blender(0, 0, 0, 128);
                renderDrawer->drawTransSprite(gfxinter->getSurface(PROGRESSFIX), gfxinter->getSurface(PROGRESSFIX), iDrawX+2, iDrawY+2);
                renderDrawer->drawTransSprite(gfxinter->getSurface(PROGRESS001+iFrame), gfxinter->getSurface(PROGRESS001+iFrame), iDrawX+2, iDrawY+2);
            }
            else {
                if (item->shouldPlaceIt()) {
                    // TODO: draw white/red (flicker)
                    int icon = READY01;
                    if (m_player->isContextMouseState(eMouseState::MOUSESTATE_PLACE)) {
                        icon = READY02;
                    }
                    renderDrawer->drawSprite(gfxinter->getSurface(icon), gfxinter->getSurface(icon), iDrawX + 3, iDrawY + 16);
                }
                else if (item->shouldDeployIt()) {
                    // TODO: draw white/red (flicker)
                    // TODO: draw DEPLOY
                    int icon = READY01;
                    if (m_player->isContextMouseState(eMouseState::MOUSESTATE_DEPLOY)) {
                        icon = READY02;
                    }
                    renderDrawer->drawSprite(bmp_screen, gfxinter->getSurface(icon), iDrawX + 3, iDrawY + 16);
                }
            }
        }
        else {   // not building
            // this item is not being built. So we do not draw a progress indicator.
            // however, it could be that an other item is being built.

            // draw the item 'unavailable' when:
            // - is not available (doh)
            // - we cant pay it
            // - some other item is being built
            // - list is being upgraded, so you cannot build items
            bool isBuildingSameSubListItem = false;
            for (int i2 = 0; i2 < 5; i2++) {
                if (isBuildingItemInList[i2] < 0) continue;
                if (i2 == item->getSubList()) {
                    isBuildingSameSubListItem = true;
                    break;
                }
            }

            if (!item->isAvailable() || isBuildingSameSubListItem) {
                // @Mira fix trasnparency set_trans_blender(0, 0, 0, 64);
                renderDrawer->drawTransSprite(gfxinter->getSurface(PROGRESSNA), gfxinter->getSurface(PROGRESSNA), iDrawX, iDrawY);

                // Pending upgrading (ie: an upgrade is progressing, blocking the construction of these items)
                if (item->isPendingUpgrading()) {
                    SDL_Color errorFadingColor = m_player->getErrorFadingColor();
                    renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);

                    SDL_Color red = SDL_Color{255, 0, 0,255};
                    m_textDrawer.setFont(small_font);
                    m_textDrawer.drawTextCenteredInBox("Upgrading", iDrawX, iDrawY, withOfIcon, heightOfIcon, red);
                    m_textDrawer.setFont(game_font);
                }

                // Pending building (ie: a build is progressing, blocking the upgrade)
                if (item->isPendingBuilding()) {
                    SDL_Color errorFadingColor = m_player->getErrorFadingColor();
                    renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);

                    SDL_Color red = SDL_Color{255, 0, 0, 255};
                    m_textDrawer.setFont(small_font);
                    int height = heightOfIcon / 3;
                    m_textDrawer.drawTextCenteredInBox("Building", iDrawX, iDrawY, withOfIcon, height, red);
                    m_textDrawer.drawTextCenteredInBox("in", iDrawX, iDrawY + height, withOfIcon, height, red);
                    m_textDrawer.drawTextCenteredInBox("progress", iDrawX, iDrawY + (height * 2), withOfIcon, height, red);
                    m_textDrawer.setFont(game_font);
                }
            }

            if (list->getType() == eListType::LIST_STARPORT) {
                // only for starport show you can't pay it, as we allow building units when you cannot pay it (ie partial
                // payment/progress)
                if (cannotPayIt) {
                    // @Mira fix trasnparency set_trans_blender(0, 0, 0, 64);
                    renderDrawer->drawTransSprite(gfxinter->getSurface(PROGRESSNA), gfxinter->getSurface(PROGRESSNA), iDrawX, iDrawY);
                    SDL_Color errorFadingColor = m_player->getErrorFadingColor();
                    renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    renderDrawer->drawLine(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);
                }
            }

            // last built id
            if (list->getLastClickedId() == i) {
                //_rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
                renderDrawer->drawRect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1)-(iDrawX + 1), (iDrawYEnd - 1)-(iDrawY + 1), selectFadingColor);
                renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, selectFadingColor);
            }
        }

        // if item is queuable, then it will show the amount of
        // items being built. Meaning, with 1 item to build, it will show a 1.
        int amountToShow = item->getTimesToBuild();
        bool showAmount = item->isQueuable() && amountToShow > 0;

        bool listIsStarport = item->getList()->getType() == eListType::LIST_STARPORT;
        if (listIsStarport) {
            amountToShow = item->getTimesOrdered();
            showAmount = true; // starport shows always all amounts
        }

        if (showAmount) {
            // draw number of times to build this thing (queueing)
            int textX = iDrawX + 41;
            int textY = iDrawY + 16;

            if (amountToShow < 10) {
                textX += 10;
            }

            // draw
            m_textDrawer.drawText(textX, textY, fmt::format("{}",amountToShow));
        }

        if (game.isDebugMode()) {
            if (m_renderListIds) {
                int textX = iDrawX + 41;
                int textY = iDrawY + 40;

                m_textDrawer.drawText(textX, textY, fmt::format("{}",item->getSubList()));
            }
        }

        // draw rectangle when mouse hovers over icon
        auto m_mouse = game.getMouse();
        if (isOverItemCoordinates_Boolean(m_mouse->getX(), m_mouse->getY(), iDrawX, iDrawY)) {
            //_rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
            renderDrawer->drawRect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1)-(iDrawX + 1), (iDrawYEnd - 1)-(iDrawY + 1), selectFadingColor);
            renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, selectFadingColor);
        }
        else {
            SDL_Color color = list->getFlashingColor();
            if (item->isFlashing()) {
                renderDrawer->drawRect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1)-(iDrawX + 1), (iDrawYEnd - 1)-(iDrawY + 1), color);
                renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iDrawXEnd-iDrawX, iDrawYEnd-iDrawY, color);
            }
        }

        // update coordinates, 3 icons in a row
        if (rowNr < 2) {
            iDrawX+=66;
            rowNr++;
        }
        else {
            rowNr = 0;
            iDrawX=getDrawX();
            iDrawY+=50;
        }
    }

    SDL_SetClipRect(bmp_screen, nullptr);
}

/**
 * Draw the amount of squares a structure would take. The X and Y coordinates are the top
 * left coordinates of an icon.
 *
 * @param structureId
 * @param x
 * @param y
 */
void cBuildingListDrawer::drawStructureSize(int structureId, int x, int y)
{
    // figure out size
    int iW= sStructureInfo[structureId].bmp_width / 32;
    int iH= sStructureInfo[structureId].bmp_height / 32;
    int iTile = GRID_1X1;

    if (iW == 2 && iH == 2) {
        iTile = GRID_2X2;
    }

    if (iW == 3 && iH == 2) {
        iTile = GRID_3X2;
    }

    if (iW == 3 && iH == 3) {
        iTile = GRID_3X3;
    }

    SDL_Surface *temp=SDL_CreateRGBSurface(0,19,19,32,0,0,0,255);

    // @Mira fix trasnparency set_trans_blender(0, 0, 0, 192);
    //? draw empty ? renderDrawer->drawTransSprite(temp, temp, x + 43, y + 20);

    renderDrawer->drawSprite(temp, gfxinter->getSurface(GRID_0X0), 0, 0);

    renderDrawer->drawTransSprite(temp, temp, x + 43, y + 20);

    renderDrawer->drawSprite(gfxinter->getSurface(iTile), gfxinter->getSurface(iTile), x + 43, y + 20);

    SDL_FreeSurface(temp);

}

bool cBuildingListDrawer::isOverItemCoordinates_Boolean(int x, int y, int drawX, int drawY)
{
    if (x >= drawX && x <= (drawX + 64) && y >= drawY && y < (drawY + 48)) {
        return true;
    }
    return false;
}

cBuildingListItem *cBuildingListDrawer::isOverItemCoordinates(cBuildingList *list, int x, int y)
{
    assert(list != nullptr);
    // starting draw coordinates

//	int iDrawX=drawManager->getSidebarDrawer()->getBuildingListDrawer()->getDrawX();
//	int iDrawY=drawManager->getSidebarDrawer()->getBuildingListDrawer()->getDrawY();
    int iDrawX=getDrawX();
    int iDrawY=getDrawY();

    int end = MAX_ITEMS;

    int rowNr = 0;
    for (int i = 0; i < end; i++) {
        cBuildingListItem *item = list->getItem(i);
        if (item == nullptr) break;

        if (isOverItemCoordinates_Boolean(x, y, iDrawX, iDrawY)) {
            return item;
        }

        if (rowNr < 2) {
            rowNr++;
            iDrawX+=66;
        }
        else {
            iDrawY += 50;
            iDrawX=getDrawX();
            rowNr = 0;
        }
    }

    return nullptr;
}

void cBuildingListDrawer::setPlayer(cPlayer *player)
{
    this->m_player = player;
}

void cBuildingListDrawer::onNotifyMouseEvent(const s_MouseEvent &)
{
    // NOOP
}

void cBuildingListDrawer::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    switch (event.eventType) {
        case eKeyEventType::HOLD:
            onKeyHold(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;
    }
}

void cBuildingListDrawer::onKeyHold(const cKeyboardEvent &event)
{
    if (event.hasKeys(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
        m_renderListIds = true;
    }
}

void cBuildingListDrawer::onKeyPressed(const cKeyboardEvent &event)
{
    if (event.hasEitherKey(SDL_SCANCODE_TAB, SDL_SCANCODE_D)) {
        m_renderListIds = false;
    }
}
