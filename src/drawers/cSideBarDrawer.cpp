#include "cSideBarDrawer.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/cAllegroDrawer.h"
#include "drawers/cOrderDrawer.h"
#include "managers/cDrawManager.h"
#include "player/cPlayer.h"

#include <allegro.h>

#include <algorithm>

cSideBarDrawer::cSideBarDrawer(cPlayer *player) :
    m_player(player),
    m_buildingListDrawer(player),
    m_sidebar(nullptr),
    m_candybar(nullptr),
    m_textDrawer(bene_font),
    m_sidebarColor(makecol(214, 149, 20))
{
    assert(player);
}

cSideBarDrawer::~cSideBarDrawer()
{
    if (m_candybar) {
        destroy_bitmap(m_candybar);
    }
    m_sidebar = nullptr;
}

void cSideBarDrawer::drawCandybar()
{
    if (m_candybar == nullptr) {
        createCandyBar();
    }

    int drawX = game.m_screenW - cSideBar::SidebarWidth;
    int drawY = 40;
    renderDrawer->drawSprite(bmp_screen, m_candybar, drawX, drawY);
}

void cSideBarDrawer::createCandyBar()
{
    int heightInPixels = (game.m_screenH - cSideBar::TopBarHeight);
    m_candybar = create_bitmap_ex(8, 24, heightInPixels);
    clear_to_color(m_candybar, makecol(0, 0, 0));

    // ball first
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BALL].dat, 0, 0); // height of ball = 25
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_TOP].dat, 0, 26); // height of top = 10
// now draw pieces untill the end (height of piece is 23 pixels)
    int startY = 26 + 10; // end of ball (26) + height of top m_candybar (=10) , makes 36
    int heightMinimap = cSideBar::HeightOfMinimap;
    set_clip_rect(m_candybar, 0, 0, 24, heightMinimap - (6 + 1)); // (add 1 pixel for room between ball and bar)
    for (int y = startY; y < (heightMinimap); y += 24) {
        renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_PIECE].dat, 0, y);
    }
    set_clip_rect(m_candybar, 0, 0, m_candybar->w, m_candybar->h);

    // note: no need to take top bar into account because 'm_candybar' is a separate bitmap so coords start at 0,0
// ball is 6 pixels higher than horizontal m_candybar
    int ballY = (heightMinimap) - 6;

    // draw bottom m_candybar
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BOTTOM].dat, 0, ballY - 10); // height of bottom = 9

    // draw ball
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BALL].dat, 0, ballY); // height of ball = 25

    // draw top m_candybar again
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_TOP].dat, 0, ballY + 26); // height of top = 10

    startY = ballY + 26 + 10;
    for (int y = startY; y < (heightInPixels + 23); y += 24) {
        renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_PIECE].dat, 0, y);
    }
    // draw bottom
    renderDrawer->drawSprite(m_candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BOTTOM].dat, 0, heightInPixels - 10); // height of top = 10
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
    BITMAP *backgroundSprite = (BITMAP *)gfxinter[BMP_GERALD_ICONLIST_BACKGROUND].dat;

    int drawX = game.m_screenW - cSideBar::SidebarWidthWithoutCandyBar + 1;

    int startY = cSideBar::TopBarHeight + cSideBar::HeightOfMinimap + cSideBar::HorizontalCandyBarHeight +
                 cSideBar::HeightOfListButton;

    for (; drawX < game.m_screenW; drawX += backgroundSprite->w) {
        for (int drawY=startY; drawY < game.m_screenH; drawY += backgroundSprite->h) {
            renderDrawer->drawSprite(bmp_screen, backgroundSprite, drawX, drawY);
        }
    }

    // draw the 'lines' between the icons    // draw the buildlist itself (take scrolling into account)
    cBuildingList *selectedList = nullptr;

    int iDrawX = m_buildingListDrawer.getDrawX();
    int iDrawY = m_buildingListDrawer.getDrawY();

    BITMAP *horBar = (BITMAP *) gfxinter[BMP_GERALD_SIDEBAR_PIECE].dat;
    renderDrawer->drawSprite(bmp_screen, horBar, iDrawX-1, iDrawY-38); // above sublist buttons
    renderDrawer->drawSprite(bmp_screen, horBar, iDrawX-1, iDrawY-5); // above normal icons

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
        int darker = makecol(89, 56, 0);
        int veryDark = makecol(48, 28, 0);
        renderDrawer->drawLine(bmp_screen, barX - 1, iDrawY, barX - 1, endY, darker);
        renderDrawer->drawLine(bmp_screen, barX, iDrawY, barX, endY, veryDark);

        // horizontal lines
        for (int j = 1; j < rows; j++) {
            int barY = iDrawY - 1 + (j * 50);
            renderDrawer->drawLine(bmp_screen, iDrawX, barY-1, game.m_screenW, barY - 1, darker);
            renderDrawer->drawLine(bmp_screen, iDrawX, barY, game.m_screenW, barY, veryDark);
        }
    }

    if (selectedList && selectedList->getType() == eListType::LIST_STARPORT) {
        renderDrawer->drawRectFilled(bmp_screen, iDrawX, endY, game.m_screenW-iDrawX, game.m_screenH-endY, m_sidebarColor);
        renderDrawer->drawSprite(bmp_screen, horBar, iDrawX-1, endY); // just below the last icons
    }

    // vertical lines at the side
    renderDrawer->drawLine(bmp_screen, iDrawX - 1, iDrawY-38, iDrawX-1, game.m_screenH, makecol(255, 211, 125)); // left
    renderDrawer->drawLine(bmp_screen, game.m_screenW - 1, iDrawY - 38, game.m_screenW - 1, endY, makecol(209, 150, 28)); // right

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
    set_palette(m_player->pal);

    // black out sidebar
    renderDrawer->drawRectFilled(bmp_screen, (game.m_screenW - cSideBar::SidebarWidth), 0, cSideBar::SidebarWidth, game.m_screenH, makecol(0, 0, 0));

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
    cRectangle powerBarRect(barX, barY, barWidth, barTotalHeight);

    renderDrawer->drawRectFilled(bmp_screen, powerBarRect, renderDrawer->getColor_BLACK());

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

    renderDrawer->drawRectFilled(bmp_screen, barX, powerInY, barWidth, barY + barTotalHeight - powerInY, makecol(0, 232, 0));

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
        renderDrawer->drawRectFilled(bmp_screen, barX, powerOutY, barWidth, barY + barTotalHeight - powerOutY, makecol(r, g, 32));
    }
    else {
        renderDrawer->drawRectFilled(bmp_screen, barX, powerOutY, barWidth, barY + barTotalHeight - powerOutY, m_player->getErrorFadingColor());
    }

    renderDrawer->drawLine(bmp_screen, barX, powerOutY, barX+barWidth, powerOutY, makecol(255, 255, 255));

    renderDrawer->drawRect(bmp_screen, powerBarRect, m_sidebarColor);

    // draw darker 'sides' at the left and top
    int darker = makecol(89, 56, 0);
    renderDrawer->drawLine(bmp_screen, barX, barY, barX, barY + barTotalHeight, darker); // left side |
    renderDrawer->drawLine(bmp_screen, barX, barY, barX+barWidth, barY, darker); // top side _

    m_textDrawer.drawText(barX - 1, barY - 21, makecol(0, 0, 0), "$");
    m_textDrawer.drawText(barX + 1, barY - 19, makecol(0, 0, 0), "$");
    m_textDrawer.drawText(barX, barY - 20, "$");
}

void cSideBarDrawer::drawPowerUsage() const
{
    int arbitraryMargin = 6;
    int barTotalHeight = game.m_screenH - (cSideBar::TotalHeightBeforePowerBarStarts + cSideBar::PowerBarMarginHeight);
    int barX = (game.m_screenW - cSideBar::SidebarWidth) + (cSideBar::VerticalCandyBarWidth / 3);
    int barY = cSideBar::TotalHeightBeforePowerBarStarts + arbitraryMargin;
    int barWidth = (cSideBar::VerticalCandyBarWidth / 3) - 1;
    cRectangle powerBarRect(barX, barY, barWidth, barTotalHeight);

    renderDrawer->drawRectFilled(bmp_screen, powerBarRect, renderDrawer->getColor_BLACK());

    // the maximum power (ie a full bar) is 1 + amount windtraps * power_give (100)
    int maxPowerOutageOfWindtrap = sStructureInfo[WINDTRAP].power_give;
    float totalPowerOutput = (1 + (m_player->getAmountOfStructuresForType(WINDTRAP))) * maxPowerOutageOfWindtrap;
    float powerIn = (float)m_player->getPowerProduced() / totalPowerOutput;
    float powerUse = (float)m_player->getPowerUsage() / totalPowerOutput;

    float barHeightToDraw = barTotalHeight * powerIn;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerInY = barY + (barTotalHeight - barHeightToDraw);

    renderDrawer->drawRectFilled(bmp_screen, barX, powerInY, barWidth, barTotalHeight, makecol(0, 232, 0));

    barHeightToDraw = barTotalHeight * powerUse;
    if (barHeightToDraw > barTotalHeight) barHeightToDraw = barTotalHeight;
    int powerOutY = barY + (barTotalHeight - barHeightToDraw);

    float powerUsageRatio = ((float)m_player->getPowerUsage() + 1) / ((float)m_player->getPowerProduced() + 1);
    int r = powerUsageRatio * 255;
    int g = (1.1 - powerUsageRatio) * 255;

    if (r > 255) r = 255;
    if (g > 255) g = 255;

    if (m_player->bEnoughPower()) {
        renderDrawer->drawRectFilled(bmp_screen, barX, powerOutY, barWidth, barY + barTotalHeight-powerOutY, makecol(r, g, 32));
    }
    else {
        renderDrawer->drawRectFilled(bmp_screen, barX, powerOutY, barWidth, barY + barTotalHeight-powerOutY, m_player->getErrorFadingColor());
    }

    renderDrawer->drawLine(bmp_screen, barX, powerOutY, barX+barWidth, powerOutY, makecol(255, 255, 255));

    renderDrawer->drawRect(bmp_screen, powerBarRect, m_sidebarColor);

    // draw darker 'sides' at the left and top
    int darker = makecol(89, 56, 0);
    renderDrawer->drawLine(bmp_screen, barX, barY, barX, barY + barTotalHeight, darker); // left side |
    renderDrawer->drawLine(bmp_screen, barX, barY, barX+barWidth, barY, darker); // top side _

    //renderDrawer->drawSprite(bmp_screen, D2TM_BITMAP_ICON_POWER, barX-3, barY - 21);
    renderDrawer->drawSprite(bmp_screen, (BITMAP *)gfxinter[3].dat,barX-3, barY - 21);
//    m_textDrawer->drawText(barX-1, barY - 21, makecol(0,0,0),"P");
//    m_textDrawer->drawText(barX+1, barY - 19, makecol(0,0,0),"P");
//    m_textDrawer->drawText(barX, barY - 20, "P");
}

void cSideBarDrawer::drawMinimap()
{
    BITMAP *sprite = (BITMAP *)gfxinter[HORIZONTAL_CANDYBAR].dat;
    int drawX = (game.m_screenW - sprite->w) + 1;
    // 128 pixels (each pixel is a cell) + 8 margin
    int heightMinimap = cSideBar::HeightOfMinimap;
    int drawY = cSideBar::TopBarHeight + heightMinimap;
    renderDrawer->drawSprite(bmp_screen, sprite, drawX, drawY);

    // 11, 10
    // 78, 60
    // ------
    // 67, 50 (width/height)

    bool hasRadarAndEnoughPower = m_player->hasRadarAndEnoughPower();

    if (hasRadarAndEnoughPower) {
        return; // bail, because we render the minimap
    }

    // else, we render the house emblem
    renderDrawer->drawRectFilled(bmp_screen, drawX + 1, cSideBar::TopBarHeight + 1,
                                 game.m_screenW-(drawX + 1), drawY-(cSideBar::TopBarHeight + 1), m_player->getEmblemBackgroundColor());

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

        renderDrawer->stretchBlit((BITMAP *) gfxinter[bitmapId].dat, bmp_screen, srcX, srcY, emblemWidth,
                                  emblemHeight, drawX, drawY, emblemDesiredWidth, emblemDesiredHeight);
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