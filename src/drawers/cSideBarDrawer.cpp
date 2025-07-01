#include "cSideBarDrawer.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cOrderDrawer.h"
#include "managers/cDrawManager.h"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>

#include <algorithm>

cSideBarDrawer::cSideBarDrawer(cPlayer *player) :
    m_player(player),
    m_buildingListDrawer(player),
    m_sidebar(nullptr),
    // m_candybar(nullptr),
    m_textDrawer(bene_font),
    m_sidebarColor(SDL_Color{214, 149, 20,255})
{
    assert(player);
}

cSideBarDrawer::~cSideBarDrawer()
{
    // if (m_candybar) {
    //     SDL_FreeSurface(m_candybar);
    // }
    // m_sidebar = nullptr;
}

void cSideBarDrawer::drawCandybar()
{
    // if (m_candybar == nullptr) {
    int drawX = game.m_screenW - cSideBar::SidebarWidth;
    int drawY = 40;
    // @Mira render to texture
    createCandyBar(drawX,drawY);
    // }
    //renderDrawer->drawSprite(bmp_screen, drawX, drawY);
}

void cSideBarDrawer::createCandyBar(int drawX, int drawY)
{
    int heightInPixels = (game.m_screenH - cSideBar::TopBarHeight);
    // m_candybar = SDL_CreateRGBSurface(0,24, heightInPixels,8,0,0,0,0);
    // renderDrawer->FillWithColor(SDL_Color{0,0,0,255});

    // ball first
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_BALL), drawX,drawY); // height of ball = 25
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_TOP), drawX, drawY+26); // height of top = 10
    // now draw pieces untill the end (height of piece is 23 pixels)
    int startY = 26 + 10; // end of ball (26) + height of top m_candybar (=10) , makes 36
    int heightMinimap = cSideBar::HeightOfMinimap;
    // auto tmp = SDL_Rect{0,0,24, heightMinimap - (6 + 1)};  // (add 1 pixel for room between ball and bar)
    // SDL_SetClipRect(&tmp);
    for (int y = startY; y < (heightMinimap); y += 24) {
        renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_PIECE), drawX, drawY+y);
    }
    // SDL_SetClipRect(nullptr);

    // note: no need to take top bar into account because 'm_candybar' is a separate bitmap so coords start at 0,0
// ball is 6 pixels higher than horizontal m_candybar
    int ballY = (heightMinimap) - 6;

    // draw bottom m_candybar
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_BOTTOM), drawX, drawY+ballY - 10); // height of bottom = 9

    // draw ball
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_BALL), drawX, drawY+ballY); // height of ball = 25

    // draw top m_candybar again
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_TOP), drawX, drawY+ballY + 26); // height of top = 10

    startY = ballY + 26 + 10;
    for (int y = startY; y < (heightInPixels + 23); y += 24) {
        renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_PIECE), drawX, drawY+y);
    }
    // draw bottom
    renderDrawer->renderSprite(gfxinter->getTexture(BMP_GERALD_CANDYBAR_BOTTOM), drawX, drawY+heightInPixels - 10); // height of top = 10
}

void cSideBarDrawer::drawBuildingLists()
{
    // draw the sidebar itself (the backgrounds, borders, etc)

    // !??! sidebar is not set earlier...but can be re-set, so keep doing this :(
    // This basically means the allocation goes wrong somewhere! :(
    m_sidebar = m_player->getSideBar();

    // draw the buildlist icons
    int selectedListId = m_sidebar->getSelectedListID();

    int startPos = eListTypeAsInt(eListType::LIST_CONSTYARD);
    for (int listId = startPos; listId < cSideBar::LIST_MAX; listId++) {
        cBuildingList *list = m_sidebar->getList(listId);
        bool isListIdSelectedList = (selectedListId == listId);
        m_buildingListDrawer.drawButton(list, isListIdSelectedList);
    }

    // draw background of buildlist
    Texture *backgroundSprite = gfxinter->getTexture(BMP_GERALD_ICONLIST_BACKGROUND);

    int drawX = game.m_screenW - cSideBar::SidebarWidthWithoutCandyBar + 1;

    int startY = cSideBar::TopBarHeight + cSideBar::HeightOfMinimap + cSideBar::HorizontalCandyBarHeight +
                 cSideBar::HeightOfListButton;

    for (; drawX < game.m_screenW; drawX += backgroundSprite->w) {
        for (int drawY=startY; drawY < game.m_screenH; drawY += backgroundSprite->h) {
            renderDrawer->renderSprite(backgroundSprite, drawX, drawY);
        }
    }

    // draw the 'lines' between the icons    // draw the buildlist itself (take scrolling into account)
    cBuildingList *selectedList = nullptr;

    int iDrawX = m_buildingListDrawer.getDrawX();
    int iDrawY = m_buildingListDrawer.getDrawY();

    Texture *horBar = gfxinter->getTexture(BMP_GERALD_SIDEBAR_PIECE);
    renderDrawer->renderSprite(horBar, iDrawX-1, iDrawY-38); // above sublist buttons
    renderDrawer->renderSprite(horBar, iDrawX-1, iDrawY-5); // above normal icons

    if (selectedListId > -1) {
        selectedList = m_sidebar->getList(selectedListId);
    }

    int endY = game.m_screenH;
    int rows = 6;
    if (selectedList && selectedList->getType() == eListType::LIST_STARPORT) {
        rows = 5;
        endY = game.m_screenH - 50;
    }

    for (int i = 1; i < 3; i++) {
        int barX = (iDrawX - 1) + (i * 66);
        SDL_Color darker = SDL_Color{89, 56, 0,255};
        SDL_Color veryDark = SDL_Color{48, 28, 0,255};
        renderDrawer->renderLine( barX - 1, iDrawY, barX - 1, endY, darker);
        renderDrawer->renderLine( barX, iDrawY, barX, endY, veryDark);

        // horizontal lines
        for (int j = 1; j < rows; j++) {
            int barY = iDrawY - 1 + (j * 50);
            renderDrawer->renderLine( iDrawX, barY-1, game.m_screenW, barY - 1, darker);
            renderDrawer->renderLine( iDrawX, barY, game.m_screenW, barY, veryDark);
        }
    }

    if (selectedList && selectedList->getType() == eListType::LIST_STARPORT) {
        renderDrawer->renderRectFillColor(iDrawX, endY, game.m_screenW-iDrawX, game.m_screenH-endY, m_sidebarColor);
        renderDrawer->renderSprite(horBar, iDrawX-1, endY); // just below the last icons
    }

    // vertical lines at the side
    renderDrawer->renderLine( iDrawX - 1, iDrawY-38, iDrawX-1, game.m_screenH, SDL_Color{255, 211, 125,255}); // left
    renderDrawer->renderLine( game.m_screenW - 1, iDrawY - 38, game.m_screenW - 1, endY, SDL_Color{209, 150, 28,255}); // right

    // END drawing icons grid

    if (selectedList) {
        m_buildingListDrawer.drawList(selectedList, selectedListId);
    }

    // button interaction
    for (int i = startPos; i < cSideBar::LIST_MAX; i++) {
        if (i == selectedListId) continue; // skip selected list for button interaction
        cBuildingList *list = m_sidebar->getList(i);

        if (list->isAvailable() == false) continue; // not available, so no interaction possible

        // render hover over border
        auto m_mouse = game.getMouse();
        if (list->isOverButton(m_mouse->getX(), m_mouse->getX())) {
            m_buildingListDrawer.drawButtonHoverRectangle(list);
        }
    }

    cOrderDrawer *orderDrawer = drawManager->getOrderDrawer();

    // allow clicking on the order button
    if (selectedList && selectedList->getType() == eListType::LIST_STARPORT) {
        orderDrawer->drawOrderButton(m_player);
    }
}

// draws the sidebar on screen
void cSideBarDrawer::draw()
{
    // set_palette(m_player->pal);

    // black out sidebar
    renderDrawer->renderRectFillColor((game.m_screenW - cSideBar::SidebarWidth), 0, cSideBar::SidebarWidth, game.m_screenH, SDL_Color{0, 0, 0,255});

    drawCandybar();

    drawMinimap();
    drawCapacities();
    drawBuildingLists();
}

void cSideBarDrawer::drawCapacities()
{
    drawPowerUsage();
    drawCreditsUsage();
}

void cSideBarDrawer::drawCreditsUsage()
{
    int barTotalHeight = (cSideBar::HeightOfMinimap - 76);
    int barX = (game.m_screenW - cSideBar::SidebarWidth) + (cSideBar::VerticalCandyBarWidth / 3);
    int barY = cSideBar::TopBarHeight + 48;
    int barWidth = (cSideBar::VerticalCandyBarWidth / 3) - 1;
    // cRectangle powerBarRect(barX, barY, barWidth, barTotalHeight);
    renderDrawer->renderRectFillColor(barX, barY, barWidth, barTotalHeight, 0,0,0,255);

    // STEFAN: 01/05/2021 -> looks like a lot of this code can be moved to the player class to retrieve max spice capacity
    // and so forth.

    // the maximum power (ie a full bar) is 1 + amount windtraps * power_give (100)
    int maxSpiceCapacity = 1000; // this is still hard coded, need to move to INI file
    int structuresWithSpice = m_player->getAmountOfStructuresForType(REFINERY) + m_player->getAmountOfStructuresForType(SILO);
    float totalSpiceCapacity = (1 + structuresWithSpice) * maxSpiceCapacity;
    float maxSpice = m_player->getMaxCredits() / totalSpiceCapacity;
    float spiceStored = static_cast<float>(m_player->getCredits()) / totalSpiceCapacity;

    float barHeightToDraw = barTotalHeight * maxSpice;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerInY = barY + (barTotalHeight - barHeightToDraw);

    renderDrawer->renderRectFillColor(barX, powerInY, barWidth, barY + barTotalHeight - powerInY, SDL_Color{0, 232, 0,255});

    barHeightToDraw = barTotalHeight * spiceStored;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerOutY = barY + (barTotalHeight - barHeightToDraw);

    auto spiceCapacityRatio = static_cast<float>(m_player->getCredits() + 1) / (m_player->getMaxCredits() + 1);
    spiceCapacityRatio = std::clamp(spiceCapacityRatio, 0.0f, 1.0f);
    int r = spiceCapacityRatio * 255;
    int g = (1.1 - spiceCapacityRatio) * 255;

    if (r > 255) r = 255;
    if (g > 255) g = 255;

    if (m_player->bEnoughSpiceCapacityToStoreCredits()) {
        renderDrawer->renderRectFillColor(barX, powerOutY, barWidth, barY + barTotalHeight - powerOutY, (Uint8)r,(Uint8)g, 32,255);
    }
    else {
        renderDrawer->renderRectFillColor(barX, powerOutY, barWidth, barY + barTotalHeight - powerOutY, m_player->getErrorFadingColor());
    }

    renderDrawer->renderLine( barX, powerOutY, barX+barWidth, powerOutY, SDL_Color{255, 255, 255,255});

    //renderDrawer->renderRectFillColor(barX, barY, barWidth, barTotalHeight, m_sidebarColor);

    // draw darker 'sides' at the left and top
    SDL_Color darker = SDL_Color{89, 56, 0,255};
    renderDrawer->renderLine( barX, barY, barX, barY + barTotalHeight, darker); // left side |
    renderDrawer->renderLine( barX, barY, barX+barWidth, barY, darker); // top side _

    m_textDrawer.drawText(barX - 1, barY - 21, SDL_Color{0, 0, 0,255}, "$");
    m_textDrawer.drawText(barX + 1, barY - 19, SDL_Color{0, 0, 0,255}, "$");
    m_textDrawer.drawText(barX, barY - 20, "$");
}

void cSideBarDrawer::drawPowerUsage() const
{
    int arbitraryMargin = 6;
    int barTotalHeight = game.m_screenH - (cSideBar::TotalHeightBeforePowerBarStarts + cSideBar::PowerBarMarginHeight);
    int barX = (game.m_screenW - cSideBar::SidebarWidth) + (cSideBar::VerticalCandyBarWidth / 3);
    int barY = cSideBar::TotalHeightBeforePowerBarStarts + arbitraryMargin;
    int barWidth = (cSideBar::VerticalCandyBarWidth / 3) - 1;
    //cRectangle powerBarRect(barX, barY, barWidth, barTotalHeight);
    renderDrawer->renderRectFillColor(barX, barY, barWidth, barTotalHeight, renderDrawer->getColor_BLACK());

    // the maximum power (ie a full bar) is 1 + amount windtraps * power_give (100)
    int maxPowerOutageOfWindtrap = sStructureInfo[WINDTRAP].power_give;
    float totalPowerOutput = (1 + (m_player->getAmountOfStructuresForType(WINDTRAP))) * maxPowerOutageOfWindtrap;
    float powerIn = (float)m_player->getPowerProduced() / totalPowerOutput;
    float powerUse = (float)m_player->getPowerUsage() / totalPowerOutput;

    float barHeightToDraw = barTotalHeight * powerIn;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerInY = barY + (barTotalHeight - barHeightToDraw);

    renderDrawer->renderRectFillColor(barX, powerInY, barWidth, barTotalHeight, SDL_Color{0, 232, 0,255});

    barHeightToDraw = barTotalHeight * powerUse;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerOutY = barY + (barTotalHeight - barHeightToDraw);

    float powerUsageRatio = ((float)m_player->getPowerUsage() + 1) / ((float)m_player->getPowerProduced() + 1);
    int r = powerUsageRatio * 255;
    int g = (1.1 - powerUsageRatio) * 255;

    if (r > 255) r = 255;
    if (g > 255) g = 255;

    if (m_player->bEnoughPower()) {
        renderDrawer->renderRectFillColor(barX, powerOutY, barWidth, barY + barTotalHeight-powerOutY, SDL_Color{(Uint8)r, (Uint8)g, 32,255});
    }
    else {
        renderDrawer->renderRectFillColor(barX, powerOutY, barWidth, barY + barTotalHeight-powerOutY, m_player->getErrorFadingColor());
    }

    renderDrawer->renderLine(barX, powerOutY, barX+barWidth, powerOutY, SDL_Color{255, 255, 255,255});

    renderDrawer->renderRectColor(barX, barY, barWidth, barTotalHeight, m_sidebarColor);

    // draw darker 'sides' at the left and top
    SDL_Color darker = SDL_Color{89, 56, 0,255};
    renderDrawer->renderLine(barX, barY, barX, barY + barTotalHeight, darker); // left side |
    renderDrawer->renderLine(barX, barY, barX+barWidth, barY, darker); // top side _
    renderDrawer->renderSprite(gfxinter->getTexture(ICON_POWER),barX-3, barY - 21);
    // m_textDrawer.drawText(barX-1, barY - 21, SDL_Color{0,0,0,255},"P");
    // m_textDrawer.drawText(barX+1, barY - 19, SDL_Color{0,0,0,255},"P");
    // m_textDrawer.drawText(barX, barY - 20, "P");
}

void cSideBarDrawer::drawMinimap()
{
    Texture *sprite = gfxinter->getTexture(HORIZONTAL_CANDYBAR);
    int drawX = (game.m_screenW - sprite->w) + 1;
    // 128 pixels (each pixel is a cell) + 8 margin
    int heightMinimap = cSideBar::HeightOfMinimap;
    int drawY = cSideBar::TopBarHeight + heightMinimap;
    renderDrawer->renderSprite(sprite, drawX, drawY);

    // 11, 10
    // 78, 60
    // ------
    // 67, 50 (width/height)

    bool hasRadarAndEnoughPower = m_player->hasRadarAndEnoughPower();

    if (hasRadarAndEnoughPower) {
        return; // bail, because we render the minimap
    }

    // else, we render the house emblem
    renderDrawer->renderRectFillColor(drawX + 1, cSideBar::TopBarHeight + 1,game.m_screenW-(drawX + 1), drawY-(cSideBar::TopBarHeight + 1),
                                      m_player->getEmblemBackgroundColor());

    if (m_player->isHouse(ATREIDES) || m_player->isHouse(HARKONNEN) || m_player->isHouse(ORDOS)) {
        int bitmapId = BMP_SELECT_HOUSE_ATREIDES;

        int srcX = 11;
        int srcY = 10;

        // draw house emblem, which is the same as the 'select house x' picture, but partially
        int emblemWidth = 68;
        int emblemHeight = 50;

        if (m_player->isHouse(HARKONNEN)) {
            bitmapId = BMP_SELECT_HOUSE_HARKONNEN;
        }

        if (m_player->isHouse(ORDOS)) {
            bitmapId = BMP_SELECT_HOUSE_ORDOS;
            emblemHeight = 49;
            srcY = 11;
        }

        int emblemDesiredWidth = (emblemWidth * 2);
        int emblemDesiredHeight = (emblemHeight * 2);

        drawX += (sprite->w / 2) - (emblemDesiredWidth / 2);
        drawY = cSideBar::TopBarHeight + ((heightMinimap / 2) - (emblemDesiredHeight / 2));

        SDL_Rect src = {srcX, srcY, emblemWidth,emblemHeight};
        SDL_Rect dest = {drawX, drawY, emblemDesiredWidth, emblemDesiredHeight};
        renderDrawer->renderStrechSprite(gfxinter->getTexture(bitmapId), src, dest);
    }
}

void cSideBarDrawer::setPlayer(cPlayer *pPlayer)
{
    m_player = pPlayer;
    m_buildingListDrawer.setPlayer(pPlayer);
}

void cSideBarDrawer::onNotifyMouseEvent(const s_MouseEvent &event)
{
    m_buildingListDrawer.onNotifyMouseEvent(event);
}

void cSideBarDrawer::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    m_buildingListDrawer.onNotifyKeyboardEvent(event);
}