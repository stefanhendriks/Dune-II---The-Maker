#include "cSetupSkirmishGameState.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/cAllegroDrawer.h"
#include "gui.h"
#include "gui/actions/cGuiActionToGameState.h"
#include "managers/cDrawManager.h"
#include "map/cMapCamera.h"
#include "map/cMapEditor.h"
#include "map/cRandomMapGenerator.h"
#include "map/cPreviewMaps.h"
#include "player/brains/cPlayerBrainSandworm.h"
#include "player/brains/cPlayerBrainSkirmish.h"
#include "player/brains/superweapon/cPlayerBrainFremenSuperWeapon.h"
#include "player/cPlayer.h"
#include "utils/cLog.h"

#include <allegro.h>
#include <fmt/core.h>
#include <algorithm>


cSetupSkirmishGameState::cSetupSkirmishGameState(cGame &theGame, std::shared_ptr<cPreviewMaps> previewMaps) : cGameState(theGame) {
    m_previewMaps = previewMaps;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[i];
        // index 0 == human player, but to keep our lives sane we don't change the index.

        // player 0 (HUMAN) is always playing,
        sSkirmishPlayer.bHuman = i == 0;

        // and 1 additional AI is minimally required to play
        sSkirmishPlayer.bPlaying = (i <= 1);

        // just some defaults
        sSkirmishPlayer.startingUnits = 3;
        sSkirmishPlayer.iCredits = 2500;
        sSkirmishPlayer.iHouse = 0; // random house
        sSkirmishPlayer.team = (i + 1); // all different team
    }

    iStartingPoints = 2;
    iSkirmishMap = -1;

    textDrawer = cTextDrawer(bene_font);

    mouse = game.getMouse();

    spawnWorms = 2;
    spawnBlooms = true;
    detonateBlooms = true;

    // Colors
    colorDarkishBackground = makecol(32, 32, 32);
    colorDarkishBorder = makecol(227, 229, 211);
    colorWhite = makecol(255, 255, 255);
    colorRed = makecol(255, 0, 0); // used for hover
    colorYellow = makecol(255, 207, 41);
    colorDarkerYellow = makecol(225, 177, 21); // a bit darker colorYellow to give some visual clue (hover color)
    colorDisabled = makecol(128, 128, 128);

    // Basic coordinates
    topBarHeight = 21;
    previewMapHeight = 129;
    previewMapWidth = 129;
    widthOfSomething = 300; //??

    // Screen
    screen_x = game.m_screenX;
    screen_y = game.m_screenY;

    // Background
    background = create_bitmap(screen_x, screen_y);
    clear_to_color(background, makecol(0, 0, 0));

    BITMAP *dunePlanet = (BITMAP *) gfxinter[BMP_GAME_DUNE].dat;
    renderDrawer->drawSprite(background, dunePlanet, game.m_screenX * 0.2, (game.m_screenY * 0.5));

    for (int dy = 0; dy < game.m_screenY; dy += 2) {
        renderDrawer->drawLine(background, 0, dy, screen_x, dy, makecol(0, 0, 0));
    }

    // Rectangles for GUI interaction
    int topBarWidth = screen_x + 4;

    // title box
    topBar = cRectangle(-1, -1, topBarWidth, topBarHeight);

    // Players title bar
    int widthOfSidebar = widthOfSomething + 2;
    int playerTitleBarWidth = screen_x - widthOfSidebar;
    int playerTitleBarHeight = topBarHeight;
    int playerTitleBarX = 0;
    int playerTitleBarY = topBarHeight;
    playerTitleBar = cRectangle(playerTitleBarX, playerTitleBarY, playerTitleBarWidth, playerTitleBarHeight);

    // this is the box at the right from the Player list
    int topRightBoxWidth = widthOfSidebar;
    int topRightBoxHeight = playerTitleBarHeight + previewMapHeight;
    int topRightBoxX = screen_x - topRightBoxWidth;
    int topRightBoxY = topBarHeight;
    topRightBox = cRectangle(topRightBoxX, topRightBoxY, topRightBoxWidth, topRightBoxHeight);

    // player list
    int playerListBarWidth = playerTitleBarWidth;
    int playerListBarHeight = topRightBoxHeight;
    int playerListBarX = 0;
    int playerListBarY = playerTitleBarY + topBarHeight;
    playerList = cRectangle(playerListBarX, playerListBarY, playerListBarWidth, playerListBarHeight);

    // map list

    // title bar
    int mapListFrameX = screen_x - topRightBoxWidth;
    int mapListFrameY = (playerListBarY + playerListBarHeight) - playerTitleBarHeight;
    int mapListFrameWidth = screen_x - mapListFrameX;
    int mapListFrameHeight = topBarHeight;

    // rectangle for map list
    mapListTitle = cRectangle(mapListFrameX, mapListFrameY, mapListFrameWidth, mapListFrameHeight);

    // actual list of maps
//    int mapListHeight = m_screenY - (topBarHeight + topRightBoxHeight + topBarHeight + topBarHeight);
    int mapListHeight = screen_y - (mapListTitle.getY() + mapListTitle.getHeight() + topBarHeight + 1);
    int mapListWidth = mapListTitle.getWidth();
    int mapListTopX = mapListTitle.getX();
    int mapListTopY = mapListTitle.getY() + mapListTitle.getHeight();

    mapList = cRectangle(mapListTopX, mapListTopY, mapListWidth, mapListHeight);

    int previewMapY = topBarHeight + 6;
    int previewMapX = screen_x - (previewMapWidth + 6);
    previewMapRect = cRectangle(previewMapX, previewMapY, previewMapWidth, previewMapHeight);

    int startPointsX = screen_x - widthOfSomething;
    int startPointsY = previewMapY;
    int startPointHitBoxWidth = 130;
    int startPointHitBoxHeight = 16;
    startPointsRect = cRectangle(startPointsX, startPointsY, startPointHitBoxWidth, startPointHitBoxHeight);

    int wormsX = screen_x - widthOfSomething;
    int wormsY = startPointsY + 32;
    int wormsHitBoxWidth = 130;
    int wormsHitBoxHeight = 16;
    wormsRect = cRectangle(wormsX, wormsY, wormsHitBoxWidth, wormsHitBoxHeight);

    int bloomsX = screen_x - widthOfSomething;
    int bloomsY = wormsY + 32;
    int bloomsHitBoxWidth = 130;
    int bloomsHitBoxHeight = 16;
    bloomsRect = cRectangle(bloomsX, bloomsY, bloomsHitBoxWidth, bloomsHitBoxHeight);

    int detonateX = screen_x - widthOfSomething;
    int detonateY = bloomsY + 32;
    int detonateHitBoxWidth = 130;
    int detonateHitBoxHeight = 16;
    detonateBloomsRect = cRectangle(detonateX, detonateY, detonateHitBoxWidth, detonateHitBoxHeight);

    int backButtonWidth = textDrawer.textLength(" BACK");
    int backButtonHeight = topBarHeight;
    int backButtonY = screen_y - topBarHeight;
    int backButtonX = 0;
    cRectangle backButtonRect(backButtonX, backButtonY, backButtonWidth, backButtonHeight);
    backButton = new cGuiButton(textDrawer, backButtonRect, " BACK");
    backButton->setRenderKind(eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER);
    cGuiActionToGameState *action = new cGuiActionToGameState(GAME_MENU, true);
    backButton->setOnLeftMouseButtonClickedAction(action);

    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;
    cRectangle startButtonRect = cRectangle(startButtonX, startButtonY, startButtonWidth, startButtonHeight);
    startButton = new cGuiButton(textDrawer, startButtonRect, "START");
    startButton->setRenderKind(eGuiButtonRenderKind::TRANSPARENT_WITHOUT_BORDER);

}

cSetupSkirmishGameState::~cSetupSkirmishGameState() {
    destroy_bitmap(background);
    background = nullptr;

    delete backButton;
}

void cSetupSkirmishGameState::thinkFast() {

}

void cSetupSkirmishGameState::draw() const {
    renderDrawer->drawSprite(bmp_screen, background, 0, 0);

    renderDrawer->gui_DrawRect(bmp_screen, topBar);

    textDrawer.drawTextCentered("Skirmish", 1);

    renderDrawer->gui_DrawRect(bmp_screen, playerTitleBar, colorDarkishBackground, colorWhite, colorWhite);
    renderDrawer->gui_DrawRect(bmp_screen, topRightBox);
    renderDrawer->gui_DrawRect(bmp_screen, playerList, colorDarkishBackground, colorWhite, colorWhite);
    renderDrawer->gui_DrawRect(bmp_screen, mapListTitle, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);
    renderDrawer->gui_DrawRect(bmp_screen, mapList);

    textDrawer.drawTextCentered("Maps", mapListTitle.getX(), mapListTitle.getWidth(), mapListTitle.getY() + 4,
                                colorYellow);

    ///////
    /// DRAW PREVIEW MAP
    //////

    // iSkirmishMap holds an index of which map to load, where index 0 means random map generated, although
    // this is only meaningful for rendering, the loading (more below) of that map does not care if it is
    // randomly generated or not.
    drawPreviewMapAndMore(previewMapRect);

    drawStartPoints(iStartingPoints, startPointsRect);

    drawWorms(wormsRect);
    drawBlooms(bloomsRect);

    drawDetonateBlooms(detonateBloomsRect);
    drawMapList(mapList);

    // Header text for players
    textDrawer.drawText(4, 25, "Player      House      Credits       Units    Team");

    // draw players who will be playing ;)
    for (int p = 0; p < (AI_WORM - 1); p++) {
        const int iDrawY = playerList.getY() + 4 + (p * 22);
        const int iDrawX = 4;

        const s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

        if (p < iStartingPoints) {
            // player playing or not

            cRectangle brainRect = cRectangle(iDrawX, iDrawY, 73, 16);
            drawPlayerBrain(sSkirmishPlayer, brainRect);

            // HOUSE
            const int houseX = 74;
            const int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);

            drawHouse(sSkirmishPlayer, houseRec);

            // Credits
            const int creditsX = 174;
            const int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);

            drawCredits(sSkirmishPlayer, creditsRect);

            // Units
            const int startingUnitsX = 272;
            const int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            drawStartingUnits(sSkirmishPlayer, startingUnitsRect);

            // Credits
            const int teamsX = 340;
            const int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            drawTeams(sSkirmishPlayer, teamsRect);
        }
    }

    cRectangle bottomBarRect = cRectangle(-1, screen_y - topBarHeight, screen_x + 2, topBarHeight + 2);
    renderDrawer->gui_DrawRect(bmp_screen, bottomBarRect);

    // For now in draw function
    startButton->setEnabled(iSkirmishMap > -1);

    backButton->draw();
    startButton->draw();

    // MOUSE
    mouse->draw();
}

void cSetupSkirmishGameState::drawTeams(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &teamsRect) const {
    int textColor = getTextColorForRect(sSkirmishPlayer, teamsRect);
    textDrawer.drawText(teamsRect.getX(), teamsRect.getY(), textColor, "%d", sSkirmishPlayer.team);
}

void
cSetupSkirmishGameState::drawStartingUnits(const s_SkirmishPlayer &sSkirmishPlayer,
                                           const cRectangle &startingUnitsRect) const {

    int textColor = getTextColorForRect(sSkirmishPlayer, startingUnitsRect);
    textDrawer.drawText(startingUnitsRect.getX(), startingUnitsRect.getY(), textColor, "%d",
                        sSkirmishPlayer.startingUnits);
}

void
cSetupSkirmishGameState::drawCredits(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &creditsRect) const {
    int textColor = getTextColorForRect(sSkirmishPlayer, creditsRect);
    textDrawer.drawText(creditsRect.getX(), creditsRect.getY(), textColor, "%d", sSkirmishPlayer.iCredits);
}

int
cSetupSkirmishGameState::getTextColorForRect(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &rect) const {
    if (rect.isPointWithin(mouse_x, mouse_y)) {
        int colorSelectedRedFade = game.getColorFadeSelected(255, 0, 0);
        int colorDisabledFade = game.getColorFadeSelected(128, 128, 128);
        return sSkirmishPlayer.bPlaying ? colorSelectedRedFade : colorDisabledFade;
    }

    if (sSkirmishPlayer.bHuman) { // should be redundant when player is always m_playing?
        return colorWhite;
    }

    return sSkirmishPlayer.bPlaying ? colorWhite : colorDisabled;
}

void cSetupSkirmishGameState::drawHouse(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &houseRec) const {
    int textColor = getTextColorForRect(sSkirmishPlayer, houseRec);
    const std::string &cPlayerHouseString = cPlayer::getHouseNameForId(sSkirmishPlayer.iHouse);
    const char *cHouse = sSkirmishPlayer.iHouse > 0 ? cPlayerHouseString.c_str() : "Random";
    textDrawer.drawText(houseRec.getX(), houseRec.getY(), textColor, cHouse);
}

void
cSetupSkirmishGameState::drawPlayerBrain(const s_SkirmishPlayer &sSkirmishPlayer, const cRectangle &brainRect) const {
    if (sSkirmishPlayer.bHuman) {
        textDrawer.drawText(brainRect.getX(), brainRect.getY(), "Human");
    } else {
        int textColor = getTextColorForRect(sSkirmishPlayer, brainRect);
        textDrawer.drawText(brainRect.getX(), brainRect.getY(), textColor, "  CPU");
    }
}

void cSetupSkirmishGameState::drawStartPoints(int iStartingPoints, const cRectangle &startPoints) const {
    int textColor = colorWhite;
    if (iSkirmishMap == 0) { // random map selected
        if (startPoints.isPointWithin(mouse_x, mouse_y)) {
            textColor = colorRed;
        }
    } else {
        textColor = colorDisabled;
    }

    textDrawer.drawText(startPoints.getX(), startPoints.getY(), textColor, "Startpoints: %d",
                                      iStartingPoints);
}

void cSetupSkirmishGameState::drawPreviewMapAndMore(const cRectangle &previewMapRect) const {
    if (iSkirmishMap > -1) {
        s_PreviewMap &selectedMap = m_previewMaps->getMap(iSkirmishMap);
        // Render skirmish map as-is (pre-loaded map)
        if (iSkirmishMap > 0) {
            if (selectedMap.name[0] != '\0') {
                if (selectedMap.terrain) {
                    draw_sprite(bmp_screen, selectedMap.terrain, previewMapRect.getX(), previewMapRect.getY());
                }
            }
        } else {
            // render the 'random generated skirmish map'

            // when mouse is hovering, draw it, else do not
            if (previewMapRect.isPointWithin(mouse_x, mouse_y)) {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, selectedMap.terrain, previewMapRect.getX(), previewMapRect.getY());
                    }
                }
            } else {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_UNKNOWNMAP].dat, previewMapRect.getX(),
                                    previewMapRect.getY());
                    }
                }
            }
        }
    }
}

void cSetupSkirmishGameState::drawDetonateBlooms(const cRectangle &detonateBloomsRect) const {
    if (spawnBlooms) {
        int textColor = detonateBloomsRect.isPointWithin(mouse_x, mouse_y) ? colorRed : colorWhite;
        textDrawer.drawText(detonateBloomsRect.getX(), detonateBloomsRect.getY(), textColor, "Auto-detonate : %s",
                            detonateBlooms
                            ? "YES" : "NO");
    } else {
        textDrawer.drawText(detonateBloomsRect.getX(), detonateBloomsRect.getY(), colorDisabled, "Auto-detonate : -");
    }
}

void cSetupSkirmishGameState::drawBlooms(const cRectangle &bloomsRect) const {
    int textColor = bloomsRect.isPointWithin(mouse_x, mouse_y) ? colorRed : colorWhite;
    textDrawer.drawText(bloomsRect.getX(), bloomsRect.getY(), textColor, "Spice blooms : %s",
                        spawnBlooms ? "YES" : "NO");
}

void cSetupSkirmishGameState::drawWorms(const cRectangle &wormsRect) const {
    int textColor = wormsRect.isPointWithin(mouse_x, mouse_y) ? colorRed : colorWhite;
    textDrawer.drawText(wormsRect.getX(), wormsRect.getY(), textColor, "Worms? : %d", spawnWorms);
}

void cSetupSkirmishGameState::prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap) {
    s_PreviewMap &selectedMap = m_previewMaps->getMap(iSkirmishMap);

    // this needs to be before setupPlayers :/
    game.m_mission = 9; // high tech level (TODO: make this customizable)

    game.setupPlayers();

    // Starting skirmish mode
    game.m_skirmish = true;

    /* set up starting positions */
    std::vector<int> iStartPositions;

    int startCellsOnSkirmishMap = 0;
    for (int s = 0; s < 5; s++) {
        int startPosition = selectedMap.iStartCell[s];
        if (startPosition < 0) continue;
        iStartPositions.push_back(startPosition);
    }

    startCellsOnSkirmishMap = iStartPositions.size();

    // REGENERATE MAP DATA FROM INFO
    map.init(selectedMap.width, selectedMap.height);

    auto mapEditor = cMapEditor(map);
    for (int c = 0; c < map.getMaxCells(); c++) {
        mapEditor.createCell(c, selectedMap.terrainType[c], 0);
    }
    mapEditor.smoothMap();

    if (game.isDebugMode()) {
        logbook("Starting positions before shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            logbook(fmt::format("iStartPositions[{}] = [{}]", i, iStartPositions[i]));
        }
    }

    logbook("Shuffling starting positions");
    std::random_shuffle(iStartPositions.begin(), iStartPositions.end());

    if (game.isDebugMode()) {
        logbook("Starting positions after shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            logbook(fmt::format("iStartPositions[{}] = [{}]", i, iStartPositions[i]));
        }
    }

    int maxThinkingAIs = MAX_PLAYERS;
    if (game.m_oneAi) {
        maxThinkingAIs = 1;
    }

    if (game.m_disableAI) {
        maxThinkingAIs = 0;
    }

    // set up players
    for (int p = 0; p < MAX_PLAYERS; p++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

        int iHouse = sSkirmishPlayer.iHouse; // get house selected, which can be 0 for RANDOM

        // not playing.. do nothing (only for playable factions)
        bool playableFaction = p < AI_CPU5;

        if (playableFaction) {
            if (!sSkirmishPlayer.bPlaying) {
                // make sure it is a brain dead AI...
                cPlayer &cPlayer = players[p];
                cPlayer.init(p, nullptr);
                continue;
            }

            // house = 0 means pick random house
            if (iHouse == 0) {
                bool bOk = false;

                while (bOk == false) {
                    if (p > HUMAN) {
                        iHouse = rnd(4) + 1;
                        // cpu player
                    } else {// human may not be sardaukar
                        iHouse = rnd(3) + 1; // hark = 1, atr = 2, ord = 3, sar = 4
                    }

                    bool houseInUse = false;
                    for (int pl = 0; pl < AI_WORM; pl++) {
                        // already in use by other skirmish set-up players
                        if (skirmishPlayer[pl].iHouse > 0 &&
                            skirmishPlayer[pl].iHouse == iHouse) {
                            houseInUse = true;
                        }

                        if (players[pl].getHouse() == iHouse) {
                            // already in use by a already-setup player
                            houseInUse = true;
                        }
                    }

                    if (!houseInUse) {
                        bOk = true;
                    }
                }
            }
        } else {
            if (p == AI_CPU5) {
                iHouse = FREMEN;
            } else {
                iHouse = GENERALHOUSE;
            }
        }

        cPlayer &pPlayer = players[p];

        // TEAM Logic
        if (p == HUMAN) {
            pPlayer.init(p, nullptr);
        } else if (p == AI_CPU5) {
            pPlayer.init(p, new brains::cPlayerBrainFremenSuperWeapon(&pPlayer));
        } else if (p == AI_CPU6) {
            if (!game.m_disableWormAi) {
                pPlayer.init(p, new brains::cPlayerBrainSandworm(&pPlayer));
            } else {
                pPlayer.init(p, nullptr);
            }
        } else {
            if (maxThinkingAIs > 0) {
                pPlayer.init(p, new brains::cPlayerBrainSkirmish(&pPlayer));
                maxThinkingAIs--;
            } else {
                pPlayer.init(p, nullptr);
            }
        }

        pPlayer.setCredits(sSkirmishPlayer.iCredits);
        pPlayer.setHouse(iHouse);

        // from here, ignore non playable factions
        if (!playableFaction) continue;

        pPlayer.setFocusCell(iStartPositions[p]);

        // Set map position
        if (p == HUMAN) {
            mapCamera->centerAndJumpViewPortToCell(pPlayer.getFocusCell());
        }

        // create constyard
        const s_PlaceResult &result = pPlayer.canPlaceStructureAt(pPlayer.getFocusCell(), CONSTYARD);
        if (!result.success) {
            // when failure, create mcv instead
            UNIT_CREATE(pPlayer.getFocusCell(), MCV, p, true);
        } else {
            pPlayer.placeStructure(pPlayer.getFocusCell(), CONSTYARD, 100);
        }
    }


    // amount of units
    int u = 0;
    int maxAmountOfStartingUnits = 0;

    for (int p = 0; p < MAX_PLAYERS; p++) {
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
        if (sSkirmishPlayer.startingUnits > maxAmountOfStartingUnits) {
            maxAmountOfStartingUnits = sSkirmishPlayer.startingUnits;
        }
    }

    // create units
    while (u < maxAmountOfStartingUnits) {
        // pick a random unit type
        int iType = rnd(12);

        for (int p = 0; p < MAX_PLAYERS; p++) {
            cPlayer &pPlayer = players[p];
            s_SkirmishPlayer &pSkirmishPlayer = skirmishPlayer[p];

            if (!pSkirmishPlayer.bPlaying) continue; // skip non playing players

            if (u >= pSkirmishPlayer.startingUnits) {
                continue; // skip this player
            }

            int iPlayerUnitType = pPlayer.getSameOrSimilarUnitType(iType);

            int minRange = 3;
            int maxRange = 12;
            int cell = map.getRandomCellFromWithRandomDistanceValidForUnitType(pPlayer.getFocusCell(),
                                                                               minRange,
                                                                               maxRange,
                                                                               iPlayerUnitType);

            UNIT_CREATE(cell, iPlayerUnitType, p, true);

            cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units",
                fmt::format("Wants {} amount of units; amount created {}", pSkirmishPlayer.startingUnits, u),
                OUTC_NONE, p, pPlayer.getHouse());
        }

        u++;
    }

    // TEAM LOGIC here, so we can decide which is Atreides and thus should be allied with Fremen...
    for (int p = 0; p < MAX_PLAYERS; p++) {
        cPlayer &player = players[p];
        s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
        if (p == HUMAN) {
            player.setTeam(sSkirmishPlayer.team);
        } else if (p == AI_CPU5) { // AI for fremen (super weapon)
            player.setTeam(AI_CPU5);
        } else if (p == AI_CPU6) {
            player.setTeam(AI_CPU6); // worm team is against everyone
        } else {
            // all other AI's are their own team (campaign == AI's are friends, here they are enemies)
            player.setTeam(sSkirmishPlayer.team);
        }
    }

    for (int p = 0; p < MAX_PLAYERS; p++) {
        cPlayer &player = players[p];
        if (player.getHouse() == ATREIDES) {
            players[AI_CPU5].setTeam(player.getTeam());
        }
    }


    // default flags (destroy everyone but me/my team)
    game.setWinFlags(3);
    game.setLoseFlags(1);

    game.playMusicByType(MUSIC_PEACE);

    map.setAutoSpawnSpiceBlooms(spawnBlooms);
    map.setAutoDetonateSpiceBlooms(detonateBlooms);
    map.setDesiredAmountOfWorms(spawnWorms);

    // spawn requested amount of worms at start
    if (spawnWorms > 0) {
        int worms = spawnWorms;
        int minDistance = worms * 12; // so on 64x64 maps this still could work
        int maxDistance = worms * 32; // 128 / 4
        int wormCell = map.getRandomCell();
        int failures = 0;
        logbook(fmt::format("Skirmish game with {} sandworms, minDistance {}, maxDistance {}", worms, minDistance, maxDistance));
        while (worms > 0) {
            int cell = map.getRandomCellFromWithRandomDistanceValidForUnitType(wormCell, minDistance, maxDistance,
                                                                               SANDWORM);
            if (cell < 0) {
                // retry
                failures++;
                if (failures > 10) {
                    // too many failed attempts, just stop
                    logbook("Failed too many times to find spot for sandworm, aborting!");
                    break;
                }
                continue;
            }
            logbook(fmt::format("Spawning sandworm at {}", cell));
            UNIT_CREATE(cell, SANDWORM, AI_WORM, true);
            wormCell = cell; // start from here to spawn new worm
            worms--;
            failures = 0; // reset failures
        }
    } else {
        logbook("Skirmish game without sandworms");
    }

    drawManager->missionInit();

    game.initiateFadingOut();
    game.setNextStateToTransitionTo(GAME_PLAYING); // this deletes the current state object
}

eGameStateType cSetupSkirmishGameState::getType() {
    return GAMESTATE_SETUP_SKIRMISH_GAME;
}

void cSetupSkirmishGameState::onNotifyMouseEvent(const s_MouseEvent &event) {
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        default:
            break;
    }

    backButton->onNotifyMouseEvent(event);
    startButton->onNotifyMouseEvent(event);
}

void cSetupSkirmishGameState::onMouseRightButtonClicked(const s_MouseEvent &) {
    onMouseRightButtonClickedAtStartPoints();
    onMouseRightButtonClickedAtWorms();
    onMouseRightButtonClickedAtPlayerList();
}

void cSetupSkirmishGameState::onMouseRightButtonClickedAtPlayerList() {// draw players who will be playing ;)
    for (int p = 0; p < (AI_WORM - 1); p++) {
        const int iDrawY = playerList.getY() + 4 + (p * 22);

        if (p < iStartingPoints) {
            s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
            if (!sSkirmishPlayer.bPlaying) continue;

            // HOUSE
            const int houseX = 74;
            const int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);
            // on click:
            if (houseRec.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.iHouse--;
                if (p > 0) {
                    if (sSkirmishPlayer.iHouse < 0) {
                        sSkirmishPlayer.iHouse = SARDAUKAR;
                    }
                } else {
                    if (sSkirmishPlayer.iHouse < 0) {
                        sSkirmishPlayer.iHouse = ORDOS;
                    }
                }
            }

            // Credits
            const int creditsX = 174;
            const int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);

            // on click:
            if (creditsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.iCredits -= 500;
                if (sSkirmishPlayer.iCredits < 1000) {
                    sSkirmishPlayer.iCredits = 10000;
                }
            }

            // Units
            const int startingUnitsX = 272;
            const int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            // on click:
            if (startingUnitsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.startingUnits--;
                if (sSkirmishPlayer.startingUnits < 1) {
                    sSkirmishPlayer.startingUnits = 10;
                }
            }

            // Credits
            const int teamsX = 340;
            const int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            // on click:
            if (teamsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.team--;
                if (sSkirmishPlayer.team < 1) {
                    sSkirmishPlayer.team = iStartingPoints;
                }
            }
        }
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClicked(const s_MouseEvent &) {
    onMouseLeftButtonClickedAtMapList();
    onMouseLeftButtonClickedAtStartPoints();
    onMouseLeftButtonClickedAtWorms();
    onMouseLeftButtonClickedAtSpawnBlooms();
    onMouseLeftButtonClickedAtDetonateBlooms();
    onMouseLeftButtonClickedAtPlayerList();
    onMouseLeftButtonClickedAtStartButton();
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtPlayerList() {
    for (int p = 0; p < (AI_WORM - 1); p++) {
        int iDrawY = playerList.getY() + 4 + (p * 22);
        int iDrawX = 4;

        if (p < iStartingPoints) {
            s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];
            // player playing or not

            cRectangle brainRect = cRectangle(iDrawX, iDrawY, 73, 16);
            // on click:
            // only allow changing 'playing' state of CPU 2 or 3 (not 1, as there should always be one
            // playing CPU)
            if (brainRect.isPointWithin(mouse_x,mouse_y)) {
                if (p > 1) {
                    if (sSkirmishPlayer.bPlaying) {
                        sSkirmishPlayer.bPlaying = false;
                    } else {
                        sSkirmishPlayer.bPlaying = true;
                    }
                }
            }

            if (!sSkirmishPlayer.bPlaying) {
                continue;
            }

            // HOUSE
            int houseX = 74;
            int houseY = iDrawY;
            cRectangle houseRec = cRectangle(houseX, houseY, 76, 16);
            // on click:
            if (houseRec.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.iHouse++;

                // Only human player can be Sardaukar?
                if (p > 0) {
                    if (sSkirmishPlayer.iHouse > SARDAUKAR) {
                        sSkirmishPlayer.iHouse = 0;
                    }
                } else {
                    if (sSkirmishPlayer.iHouse > ORDOS) {
                        sSkirmishPlayer.iHouse = 0;
                    }
                }
            }

            // Credits
            int creditsX = 174;
            int creditsY = iDrawY;
            cRectangle creditsRect = cRectangle(creditsX, creditsY, 56, 16);
            // on click:
            if (creditsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.iCredits += 500;
                if (sSkirmishPlayer.iCredits > 10000) {
                    sSkirmishPlayer.iCredits = 1000;
                }
            }

            // Units
            int startingUnitsX = 272;
            int startingUnitsY = iDrawY;
            cRectangle startingUnitsRect = cRectangle(startingUnitsX, startingUnitsY, 21, 16);
            // on click:
            if (startingUnitsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.startingUnits++;
                if (sSkirmishPlayer.startingUnits > 10) {
                    sSkirmishPlayer.startingUnits = 1;
                }
            }

            // Credits
            int teamsX = 340;
            int teamsY = iDrawY;
            cRectangle teamsRect = cRectangle(teamsX, teamsY, 21, 16);
            //  on click:
            if (teamsRect.isPointWithin(mouse_x, mouse_y)) {
                sSkirmishPlayer.team++;
                if (sSkirmishPlayer.team > iStartingPoints) {
                    sSkirmishPlayer.team = 1;
                }
            }
        }
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtDetonateBlooms() {
    if (spawnBlooms) {
        if (detonateBloomsRect.isPointWithin(mouse_x, mouse_y)) {
            detonateBlooms = !detonateBlooms;
        }
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtSpawnBlooms() {
    if (bloomsRect.isPointWithin(mouse_x, mouse_y)) {
        spawnBlooms = !spawnBlooms;
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtWorms() {
    if (wormsRect.isPointWithin(mouse_x, mouse_y)) {
        spawnWorms += 1;
        if (spawnWorms > 4) {
            spawnWorms = 0;
        }
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtStartButton() {
    int topBarHeight = 21;
    int screen_y = game.m_screenY;
    int screen_x = game.m_screenX;

    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {
        // START
        if (iSkirmishMap > -1) {
            prepareSkirmishGameToPlayAndTransitionToCombatState(iSkirmishMap);
        }
    } // mouse hovers over "START"

}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtStartPoints() {
    if (iSkirmishMap == 0) { // random map selected
        if (startPointsRect.isPointWithin(mouse_x, mouse_y)) {
            iStartingPoints++;

            if (iStartingPoints > 4) {
                iStartingPoints = 2;
            }

            generateRandomMap();
        }
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClickedAtMapList() {
    int const topBarHeight = 21;
    int const margin = 2;
    int const mapItemButtonHeight = topBarHeight;
    int const mapItemButtonWidth = mapList.getWidth() - (margin * 2);
    int const iDrawX = mapList.getX() + margin;


    // yes, this means higher resolutions can show more maps.. for now
    int maxMapsInList = std::min(((mapList.getHeight() / mapItemButtonHeight) - 1), MAX_SKIRMISHMAPS);

    // for every map that we read , draw here
    for (int i = 0; i < maxMapsInList; i++) {
        s_PreviewMap &previewMap = m_previewMaps->getMap(i);
        if (previewMap.name.empty()) continue;

        int iDrawY = mapList.getY() + (i * mapItemButtonHeight) + i;

        bool bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);

        if (bHover && previewMap.validMap) {
            iSkirmishMap = i;

            if (i == 0) {
                generateRandomMap();
            } else {
                if (previewMap.name[0] != '\0') {
                    iStartingPoints = 0;
                    // count starting points
                    for (int s: previewMap.iStartCell) {
                        if (s > -1) {
                            iStartingPoints++;
                        }
                    }
                }
            }
        }
    }
}

void cSetupSkirmishGameState::generateRandomMap() {
    randomMapGenerator.generateRandomMap(iStartingPoints, m_previewMaps->getMap(0) );
    spawnWorms = map.isBigMap() ? 4 : 2;
}

void cSetupSkirmishGameState::drawMapList(const cRectangle &mapList) const {
    int const margin = 2;
    int const mapItemButtonHeight = topBarHeight;
    int const mapItemButtonWidth = mapList.getWidth() - (margin*2);
    int iDrawX = mapList.getX() + margin;

    // yes, this means higher resolutions can show more maps.. for now
    int maxMapsInList = std::min(((mapList.getHeight() / mapItemButtonHeight)-1), MAX_SKIRMISHMAPS);

    // for every map that we read , draw here
    for (int i = 0; i < maxMapsInList; i++) {
        s_PreviewMap &previewMap = m_previewMaps->getMap(i);
        if (previewMap.name.empty()) continue;
        int iDrawY = mapList.getY() + (i * mapItemButtonHeight) + i;

        bool bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);

        int textColor = bHover ? colorRed : colorWhite;
        if (!previewMap.validMap) {
            textColor = colorDisabled;
        }

        if (bHover && previewMap.validMap && mouse->isLeftButtonClicked()) {
            GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);
        }

        // selected map, always render as pressed
        if (i == iSkirmishMap) {
            textColor = bHover ? colorDarkerYellow : colorYellow;
            if (!previewMap.validMap) {
                textColor = colorDisabled;
            }
            GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapItemButtonWidth, mapItemButtonHeight);
        }

        textDrawer.drawText(mapList.getX() + 4, iDrawY + 4, textColor, previewMap.name.c_str());
    }
}

void cSetupSkirmishGameState::onMouseRightButtonClickedAtStartPoints() {
    if (iSkirmishMap == 0) { // random map selected
        if (startPointsRect.isPointWithin(mouse_x, mouse_y)) {
            iStartingPoints--;

            if (iStartingPoints < 2) { // < 2 startpoints is not allowed
                iStartingPoints = 4; // wrap around to max
            }

            generateRandomMap();
        }
    }
}

void cSetupSkirmishGameState::onMouseRightButtonClickedAtWorms() {
    if (wormsRect.isPointWithin(mouse_x, mouse_y)) {
        spawnWorms -= 1;
        if (spawnWorms < 0) {
            spawnWorms = 4;
        }
    }
}


void cSetupSkirmishGameState::onMouseMovedTo(const s_MouseEvent &) {

}

void cSetupSkirmishGameState::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.hasKey(KEY_ESC)) {
            game.setNextStateToTransitionTo(GAME_MENU);
            game.initiateFadingOut();
        }
    }
}

