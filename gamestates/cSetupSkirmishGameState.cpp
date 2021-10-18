#include <algorithm>
#include "d2tmh.h"


cSetupSkirmishGameState::cSetupSkirmishGameState(cGame &theGame) : cGameState(theGame) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        // index 0 == human player, but to keep our lives sane we don't change the index.

        // player 0 (HUMAN) is always playing,
        // and 1 additional AI is minimally required to play
        skirmishPlayer[i].bPlaying = (i <= 1);

        // just some defaults
        skirmishPlayer[i].startingUnits = 3;
        skirmishPlayer[i].iCredits = 2500;
        skirmishPlayer[i].iHouse = 0; // random house
        skirmishPlayer[i].team = (i+1); // all different team
    }
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

    // Basic coordinates
    topBarHeight = 21;
    previewMapHeight = 129;
    previewMapWidth = 129;
    widthOfSomething = 300; //??

    // Screen
    screen_x = game.screen_x;
    screen_y = game.screen_y;
}

cSetupSkirmishGameState::~cSetupSkirmishGameState() {
}

void cSetupSkirmishGameState::thinkFast() {

}

void cSetupSkirmishGameState::draw() const {
    int screen_x = game.screen_x;
    int screen_y = game.screen_y;

    bool bFadeOut=false;

    draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_GAME_DUNE].dat, game.screen_x * 0.2, (game.screen_y * 0.5));

    for (int dy=0; dy < game.screen_y; dy+=2) {
        line(bmp_screen, 0, dy, screen_x, dy, makecol(0,0,0));
    }

    int topBarWidth = screen_x + 4;

    // title box
    cRectangle topBar = cRectangle(-1, -1, topBarWidth, topBarHeight);
    allegroDrawer->gui_DrawRect(bmp_screen, topBar);

    textDrawer.drawTextCentered("Skirmish", 1);

    int topRightBoxWidth = widthOfSomething + 2;

    // Players title bar
    int playerTitleBarWidth = screen_x - topRightBoxWidth;
    int playerTitleBarHeight = topBarHeight;
    int playerTitleBarX = 0;
    int playerTitleBarY = topBarHeight;
    cRectangle playerTitleBar = cRectangle(playerTitleBarX, playerTitleBarY, playerTitleBarWidth, playerTitleBarHeight);
    allegroDrawer->gui_DrawRect(bmp_screen, playerTitleBar, colorDarkishBackground, colorWhite, colorWhite);

    // this is the box at the right from the Player list
    int topRightBoxHeight = playerTitleBarHeight + previewMapHeight;
    int topRightBoxX = screen_x - topRightBoxWidth;
    int topRightBoxY = topBarHeight;
    cRectangle topRightBox = cRectangle(topRightBoxX, topRightBoxY, topRightBoxWidth, topRightBoxHeight);
    allegroDrawer->gui_DrawRect(bmp_screen, topRightBox);

    // player list
    int playerListBarWidth = playerTitleBarWidth;
    int playerListBarHeight = topRightBoxHeight;
    int playerListBarX = 0;
    int playerListBarY = playerTitleBarY + topBarHeight;
    cRectangle playerList = cRectangle(playerListBarX, playerListBarY, playerListBarWidth, playerListBarHeight);
    allegroDrawer->gui_DrawRect(bmp_screen, playerList, colorDarkishBackground, colorWhite, colorWhite);

    // map list
    int mapListHeight = screen_y - (topBarHeight + topRightBoxHeight + topBarHeight + topBarHeight);
    int mapListWidth = topRightBoxWidth;
    int mapListTopX = screen_x - mapListWidth;
    int mapListTopY = topRightBoxY + topRightBoxHeight; // ??

    int mapListFrameX = screen_x - mapListWidth;
    int mapListFrameY = (playerListBarY + playerListBarHeight) - playerTitleBarHeight;
    int mapListFrameWidth = screen_x - mapListFrameX;
    int mapListFrameHeight = topBarHeight;

    // rectangle for map list
    cRectangle mapListTop = cRectangle(mapListTopX, mapListTopY, mapListWidth, mapListHeight);
    allegroDrawer->gui_DrawRect(bmp_screen, mapListTop, colorDarkishBackground, colorDarkishBorder, colorDarkishBorder);

    cRectangle mapListFrame = cRectangle(mapListFrameX, mapListFrameY, mapListFrameWidth, mapListFrameHeight);

    int previewMapY = topBarHeight + 6;
    int previewMapX = screen_x - (previewMapWidth + 6);

    textDrawer.drawTextCentered("Maps", mapListFrameX, mapListFrameWidth, mapListFrameY + 4, colorYellow);

    int iStartingPoints=0;

    ///////
    /// DRAW PREVIEW MAP
    //////

    // iSkirmishMap holds an index of which map to load, where index 0 means random map generated, although
    // this is only meaningful for rendering, the loading (more below) of that map does not care if it is
    // randomly generated or not.
    int iSkirmishMap = game.iSkirmishMap;
    if (iSkirmishMap > -1) {
        s_PreviewMap &selectedMap = PreviewMap[iSkirmishMap];
        // Render skirmish map as-is (pre-loaded map)
        if (iSkirmishMap > 0) {
            if (selectedMap.name[0] != '\0') {
                if (selectedMap.terrain) {
                    draw_sprite(bmp_screen, selectedMap.terrain, previewMapX, previewMapY);
                }

                // count starting points
                for (int s : selectedMap.iStartCell) {
                    if (s > -1) {
                        iStartingPoints++;
                    }
                }
            }
        } else {
            // render the 'random generated skirmish map'
            iStartingPoints = game.iSkirmishStartPoints;

            // when mouse is hovering, draw it, else do not
            if ((mouse_x >= previewMapX && mouse_x < (previewMapX + previewMapWidth) && (mouse_y >= previewMapY && mouse_y < (previewMapY + previewMapHeight))))
            {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, selectedMap.terrain, previewMapX, previewMapY);
                    }
                }
            }
            else
            {
                if (selectedMap.name[0] != '\0') {
                    if (selectedMap.terrain) {
                        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat, previewMapX, previewMapY);
                    }
                }
            }
        }
    }

    int startPointsX = screen_x - widthOfSomething;
    int startPointsY = previewMapY;

    bool bDoRandomMap = false;

    if (game.iSkirmishMap == 0) {
        int startPointHitBoxWidth = 130;
        int startPointHitBoxHeight = 16;
        textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, "Startpoints: %d", iStartingPoints);

        if ((mouse_x >= startPointsX && mouse_x <= (startPointsX + startPointHitBoxWidth)) &&
            (mouse_y >= startPointsY && mouse_y <= (startPointsY + startPointHitBoxHeight))) {
            textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, colorRed, "Startpoints: %d",
                                              iStartingPoints);

            if (mouse->isLeftButtonClicked()) {
                game.iSkirmishStartPoints++;

                if (game.iSkirmishStartPoints > 4) {
                    game.iSkirmishStartPoints = 2;
                }

                bDoRandomMap = true;
            }

            if (mouse->isRightButtonClicked()) {
                game.iSkirmishStartPoints--;

                if (game.iSkirmishStartPoints < 2) {
                    game.iSkirmishStartPoints = 4;
                }

                bDoRandomMap = true;
            }
        }
    } else {
        textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, makecol(128, 128, 128), "Startpoints: %d",
                                          iStartingPoints);
    }

    int wormsX = screen_x - widthOfSomething;
    int wormsY = startPointsY + 32;
    int wormsHitBoxWidth = 130;
    int wormsHitBoxHeight = 16;

    textDrawer.drawText(wormsX, wormsY, "Worms? : %d", spawnWorms);

    if ((mouse_x >= wormsX && mouse_x <= (wormsX + wormsHitBoxWidth)) && (mouse_y >= wormsY && mouse_y <= (wormsY + wormsHitBoxHeight)))
    {
        textDrawer.drawText(wormsX, wormsY, colorRed, "Worms? : %d", spawnWorms);

        if (mouse->isLeftButtonClicked())
        {
//            spawnWorms += 1;
            if (spawnWorms > 4) {
//                spawnWorms = 0;
            }
        }
        if (mouse->isRightButtonClicked())
        {
//            spawnWorms -= 1;
            if (spawnWorms < 0) {
//                spawnWorms = 4;
            }
        }
    }

    int bloomsX = screen_x - widthOfSomething;
    int bloomsY = wormsY + 32;
    int bloomsHitBoxWidth = 130;
    int bloomsHitBoxHeight = 16;

    textDrawer.drawText(bloomsX, bloomsY, "Spice blooms : %s", spawnBlooms ? "YES" : "NO");

    if ((mouse_x >= bloomsX && mouse_x <= (bloomsX + bloomsHitBoxWidth)) && (mouse_y >= bloomsY && mouse_y <= (bloomsY + bloomsHitBoxHeight)))
    {
        textDrawer.drawText(bloomsX, bloomsY, colorRed, "Spice blooms : %s", spawnBlooms ? "YES" : "NO");

        if (mouse->isLeftButtonClicked())
        {
//            spawnBlooms = !spawnBlooms;
        }
    }

    int detonateX = screen_x - widthOfSomething;
    int detonateY = bloomsY + 32;
    if (spawnBlooms) {
        int detonateHitBoxWidth = 130;
        int detonateHitBoxHeight = 16;

        textDrawer.drawText(detonateX, detonateY, "Auto-detonate : %s", detonateBlooms ? "YES" : "NO");

        if ((mouse_x >= detonateX && mouse_x <= (detonateX + detonateHitBoxWidth)) && (mouse_y >= detonateY && mouse_y <= (detonateY + detonateHitBoxHeight)))
        {
            textDrawer.drawText(detonateX, detonateY, colorRed, "Auto-detonate : %s", detonateBlooms ? "YES" : "NO");

            if (mouse->isLeftButtonClicked())
            {
//                detonateBlooms = !detonateBlooms;
            }
        }
    } else {
        textDrawer.drawText(detonateX, detonateY, makecol(128, 128, 128), "Auto-detonate : -");
    }

    drawMapList(mapListTop, mapListFrame);

    int const iHeightPixels = topBarHeight;

    // Header text for players
    alfont_textprintf(bmp_screen, bene_font, 4, 26, makecol(0,0,0), "Player      House      Credits       Units    Team");
    alfont_textprintf(bmp_screen, bene_font, 4, 25, colorWhite, "Player      House      Credits       Units    Team");

    bool bHover=false;

    int selectedRedFadeColor = game.getColorFadeSelected(255, 0, 0);
    int disabledFadeColor = game.getColorFadeSelected(128, 128, 128);

    // draw players who will be playing ;)
    for (int p=0; p < (AI_WORM-1); p++)	{
        int iDrawY=playerListBarY + 4 +(p*22);
        const s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

        if (p < iStartingPoints) {
            // player playing or not
            if (p == HUMAN)	{
                alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "Human");
                alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, colorWhite, "Human");
            } else {
                alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "  CPU");

                // move hovers over... :/
                if ((mouse_x >= 4 && mouse_x <= 73) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16))) {
                    if (sSkirmishPlayer.bPlaying) {
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, selectedRedFadeColor, "  CPU");
                    } else {
                        // not available
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, disabledFadeColor, "  CPU");
                    }

                    // only allow changing 'playing' state of CPU 2 or 3 (not 1, as there should always be one
                    // playing CPU)
//                    if (p > 1 && mouse->isLeftButtonClicked())	{
//                        if (sSkirmishPlayer.bPlaying) {
//                            sSkirmishPlayer.bPlaying = false;
//                        } else {
//                            sSkirmishPlayer.bPlaying = true;
//                        }
//                    }
                }
                else
                {
                    if (sSkirmishPlayer.bPlaying)
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, colorWhite, "  CPU");
                    else
                        alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(128,128,128), "  CPU");
                }
            }

            // HOUSE
            bHover=false;
            char cHouse[30];
            memset(cHouse, 0, sizeof(cHouse));

            cPlayer &cPlayer = players[p];


            if (sSkirmishPlayer.iHouse > 0) {
                sprintf(cHouse, cPlayer::getHouseNameForId(sSkirmishPlayer.iHouse).c_str());
            } else {
                sprintf(cHouse, "Random");
            }

            int houseX = 74;
            alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY + 1, makecol(0, 0, 0), "%s", cHouse);

            if ((mouse_x >= houseX && mouse_x <= houseX+76) && (mouse_y >= iDrawY && mouse_y <= (iDrawY + 16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY, colorWhite, "%s", cHouse);
            }
            else
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY, colorWhite, "%s", cHouse);
                else
                    alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY, makecol(128, 128, 128), "%s", cHouse);

            }

            if (bHover)
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY, selectedRedFadeColor, "%s", cHouse);
                else
                    alfont_textprintf(bmp_screen, bene_font, houseX, iDrawY, disabledFadeColor, "%s", cHouse);


//                if (mouse->isLeftButtonClicked())
//                {
//                    sSkirmishPlayer.iHouse++;
//
//                    // Only human player can be Sardaukar?
//                    if (p > 0)
//                    {
//                        if (sSkirmishPlayer.iHouse > SARDAUKAR) {
//                            sSkirmishPlayer.iHouse = 0;
//                        }
//                    }
//                    else
//                    {
//                        if (sSkirmishPlayer.iHouse > ORDOS) {
//                            sSkirmishPlayer.iHouse = 0;
//                        }
//                    }
//                }

//                if (mouse->isRightButtonClicked())
//                {
//                    sSkirmishPlayer.iHouse--;
//                    if (p > 0)
//                    {
//                        if (sSkirmishPlayer.iHouse < 0) {
//                            sSkirmishPlayer.iHouse = SARDAUKAR;
//                        }
//                    }
//                    else
//                    {
//                        if (sSkirmishPlayer.iHouse < 0) {
//                            sSkirmishPlayer.iHouse =ORDOS;
//                        }
//                    }
//                }
            }

            // Credits
            bHover=false;

            int creditsTextX = 174;
            alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY + 1, makecol(0, 0, 0), "%d", sSkirmishPlayer.iCredits);

            //rect(bmp_screen, 174, iDrawY, 230, iDrawY+16, makecol(255,255,255));

            if ((mouse_x >= creditsTextX && mouse_x <= creditsTextX+56) && (mouse_y >= iDrawY && mouse_y <= (iDrawY + 16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY, colorWhite, "%d", sSkirmishPlayer.iCredits);
            }
            else
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY, colorWhite, "%d", sSkirmishPlayer.iCredits);
                else
                    alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY, makecol(128, 128, 128), "%d", sSkirmishPlayer.iCredits);

            }

            if (bHover)
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY, selectedRedFadeColor, "%d", sSkirmishPlayer.iCredits);
                else
                    alfont_textprintf(bmp_screen, bene_font, creditsTextX, iDrawY, disabledFadeColor, "%d", sSkirmishPlayer.iCredits);

//                if (mouse->isLeftButtonClicked())
//                {
//                    sSkirmishPlayer.iCredits += 500;
//                    if (sSkirmishPlayer.iCredits > 10000) {
//                        sSkirmishPlayer.iCredits = 1000;
//                    }
//                }
//
//                if (mouse->isRightButtonClicked())
//                {
//                    sSkirmishPlayer.iCredits -= 500;
//                    if (sSkirmishPlayer.iCredits < 1000) {
//                        sSkirmishPlayer.iCredits = 10000;
//                    }
//                }
            }

            // Units
            bHover = false;

            int startingUnitsX = 272;
            alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY + 1, makecol(0, 0, 0), "%d", sSkirmishPlayer.startingUnits);

            //rect(bmp_screen, 269, iDrawY, 290, iDrawY+16, makecol(255,255,255));

            if ((mouse_x >= startingUnitsX && mouse_x <= startingUnitsX+21) && (mouse_y >= iDrawY && mouse_y <= (iDrawY + 16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY, colorWhite, "%d", sSkirmishPlayer.startingUnits);
            }
            else
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY, colorWhite, "%d", sSkirmishPlayer.startingUnits);
                else
                    alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY, makecol(128, 128, 128), "%d", sSkirmishPlayer.startingUnits);

            }

            if (bHover)
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY, selectedRedFadeColor, "%d", sSkirmishPlayer.startingUnits);
                else
                    alfont_textprintf(bmp_screen, bene_font, startingUnitsX, iDrawY, disabledFadeColor, "%d", sSkirmishPlayer.startingUnits);

//                if (mouse->isLeftButtonClicked())
//                {
//                    sSkirmishPlayer.startingUnits++;
//                    if (sSkirmishPlayer.startingUnits > 10) {
//                        sSkirmishPlayer.startingUnits = 1;
//                    }
//                }
//
//                if (mouse->isRightButtonClicked())
//                {
//                    sSkirmishPlayer.startingUnits--;
//                    if (sSkirmishPlayer.startingUnits < 1) {
//                        sSkirmishPlayer.startingUnits = 10;
//                    }
//                }
            }

            // Team
            bHover=false;

            int teamsX = 340;
            alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY + 1, makecol(0, 0, 0), "%d", sSkirmishPlayer.team);

            if ((mouse_x >= teamsX && mouse_x <= teamsX+21) && (mouse_y >= iDrawY && mouse_y <= (iDrawY + 16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY, colorWhite, "%d", sSkirmishPlayer.team);
            }
            else
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY, colorWhite, "%d", sSkirmishPlayer.team);
                else
                    alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY, makecol(128, 128, 128), "%d", sSkirmishPlayer.team);

            }

            if (bHover)
            {
                if (sSkirmishPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY, selectedRedFadeColor, "%d", sSkirmishPlayer.team);
                else
                    alfont_textprintf(bmp_screen, bene_font, teamsX, iDrawY, disabledFadeColor, "%d", sSkirmishPlayer.team);

//                if (mouse->isLeftButtonClicked())
//                {
//                    sSkirmishPlayer.team++;
//                    if (sSkirmishPlayer.team > iStartingPoints) {
//                        sSkirmishPlayer.team = 1;
//                    }
//                }
//
//                if (mouse->isRightButtonClicked())
//                {
//                    sSkirmishPlayer.team--;
//                    if (sSkirmishPlayer.team < 1) {
//                        sSkirmishPlayer.team = iStartingPoints;
//                    }
//                }
            }
        }
    }

    GUI_DRAW_FRAME(-1, screen_y - topBarHeight, screen_x + 2, topBarHeight + 2);


    // back
    int backButtonWidth = textDrawer.textLength(" BACK");
    int backButtonHeight = topBarHeight;
    int backButtonY = screen_y - topBarHeight;
    int backButtonX = 0;
    textDrawer.drawTextBottomLeft(" BACK");

    // start
    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;

    textDrawer.drawTextBottomRight("START");

    if (bDoRandomMap) {
//        randomMapGenerator.generateRandomMap();
//        spawnWorms = map.isBigMap() ? 4 : 2;
    }

    // back
    if (MOUSE_WITHIN_RECT(backButtonX, backButtonY, backButtonWidth, backButtonHeight)) {
        textDrawer.drawTextBottomLeft(colorRed, " BACK");
    }

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {
        if (iSkirmishMap > -1) {
            textDrawer.drawTextBottomRight(colorRed, "START");
        } else {
            textDrawer.drawTextBottomRight(makecol(128, 128, 128), "START");
        }
    } // mouse hovers over "START"

    // MOUSE
    mouse->draw();

    if (DEBUGGING && key[KEY_TAB]) {
        textDrawer.drawTextWithTwoIntegers(mouse_x+16, mouse_y+16, "%d,%d", mouse_x, mouse_y);
    }

    if (bFadeOut) {
        game.START_FADING_OUT();
    }
}

void cSetupSkirmishGameState::prepareSkirmishGameToPlayAndTransitionToCombatState(int iSkirmishMap) {
    s_PreviewMap &selectedMap = PreviewMap[iSkirmishMap];

    // this needs to be before setup_players :/
    game.iMission=9; // high tech level (TODO: make this customizable)

    game.setup_players();

    // Starting skirmish mode
    game.bSkirmish=true;

    /* set up starting positions */
    std::vector<int> iStartPositions;

    int startCellsOnSkirmishMap=0;
    for (int s=0; s < 5; s++) {
        int startPosition = selectedMap.iStartCell[s];
        if (startPosition < 0) continue;
        iStartPositions.push_back(startPosition);
    }

    startCellsOnSkirmishMap = iStartPositions.size();

    // REGENERATE MAP DATA FROM INFO
    map.init(selectedMap.width, selectedMap.height);

    for (int c=0; c < map.getMaxCells(); c++) {
        mapEditor.createCell(c, selectedMap.mapdata[c], 0);
    }

    mapEditor.smoothMap();

    if (DEBUGGING) {
        logbook("Starting positions before shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            char msg[255];
            sprintf(msg, "iStartPositions[%d] = [%d]", i, iStartPositions[i]);
            logbook(msg);
        }
    }

    logbook("Shuffling starting positions");
    std::random_shuffle(iStartPositions.begin(), iStartPositions.end());

    if (DEBUGGING) {
        logbook("Starting positions after shuffling:");
        for (int i = 0; i < startCellsOnSkirmishMap; i++) {
            char msg[255];
            sprintf(msg, "iStartPositions[%d] = [%d]", i, iStartPositions[i]);
            logbook(msg);
        }
    }

    int maxThinkingAIs = MAX_PLAYERS;
    if (game.bOneAi) {
        maxThinkingAIs = 1;
    }

    if (game.bDisableAI) {
        maxThinkingAIs = 0;
    }

    // set up players
    for (int p = 0; p < MAX_PLAYERS; p++)	{
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
                bool bOk=false;

                while (bOk == false) {
                    if (p > HUMAN) {
                        iHouse = rnd(4)+1;
                        // cpu player
                    } else {// human may not be sardaukar
                        iHouse = rnd(3) + 1; // hark = 1, atr = 2, ord = 3, sar = 4
                    }

                    bool houseInUse=false;
                    for (int pl=0; pl < AI_WORM; pl++) {
                        // already in use by other skirmish set-up players
                        if (skirmishPlayer[pl].iHouse > 0 &&
                            skirmishPlayer[pl].iHouse == iHouse) {
                            houseInUse=true;
                        }

                        if (players[pl].getHouse() == iHouse) {
                            // already in use by a already-setup player
                            houseInUse = true;
                        }
                    }

                    if (!houseInUse) {
                        bOk=true;
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
            pPlayer.init(p, new brains::cPlayerBrainSandworm(&pPlayer));
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
    int u=0;
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

            char msg[255];
            sprintf(msg, "Wants %d amount of units; amount created %d", pSkirmishPlayer.startingUnits, u);
            cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units", msg, OUTC_NONE, p, pPlayer.getHouse());
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

    playMusicByType(MUSIC_PEACE);

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
        char msg[255];
        sprintf(msg, "Skirmish game with %d sandworms, minDistance %d, maxDistance %d", worms, minDistance, maxDistance);
        logbook(msg);
        while (worms > 0) {
            int cell = map.getRandomCellFromWithRandomDistanceValidForUnitType(wormCell, minDistance, maxDistance, SANDWORM);
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
            char msg[255];
            sprintf(msg, "Spawning sandworm at %d", cell);
            logbook(msg);
            UNIT_CREATE(cell, SANDWORM, AI_WORM, true);
            wormCell = cell; // start from here to spawn new worm
            worms--;
            failures = 0; // reset failures
        }
    } else {
        logbook("Skirmish game without sandworms");
    }

    drawManager->getMessageDrawer()->initCombatPosition();

    game.START_FADING_OUT();
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
    }
}

void cSetupSkirmishGameState::onMouseLeftButtonClicked(const s_MouseEvent &event) {
    int topBarHeight = 21;
    int screen_y = game.screen_y;
    int screen_x = game.screen_x;

    int backButtonWidth = textDrawer.textLength(" BACK");
    int backButtonHeight = topBarHeight;
    int backButtonY = screen_y - topBarHeight;
    int backButtonX = 0;

    if (MOUSE_WITHIN_RECT(backButtonX, backButtonY, backButtonWidth, backButtonHeight)) {
        game.START_FADING_OUT();
        game.setNextStateToTransitionTo(GAME_MENU); // this deletes the current state object, so has to be last statement!
        return;
    }

    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {
        int iSkirmishMap = game.iSkirmishMap;
        // START
        if (iSkirmishMap > -1) {
            prepareSkirmishGameToPlayAndTransitionToCombatState(iSkirmishMap);
            return;
        }
    } // mouse hovers over "START"
}

void cSetupSkirmishGameState::drawMapList(cRectangle &mapList, cRectangle &mapListFrame) const {
    int const iHeightPixels = topBarHeight;
    int iSkirmishMap = game.iSkirmishMap;
    int iDrawY = -1;
    int iDrawX = screen_x - widthOfSomething;
    int iEndX = screen_y;
    int iColor = colorWhite;

    // yes, this means higher resolutions can show more maps.. for now
    int maxMapsInList=std::min((mapList.getHeight() / iHeightPixels), MAX_SKIRMISHMAPS);

    // for every map that we read , draw here
    for (int i=0; i < maxMapsInList; i++) {
        if (PreviewMap[i].name[0] != '\0') {
            iDrawY=mapListFrame.getY()+(i*iHeightPixels)+i+iHeightPixels; // skip 1 bar because the 1st = 'random map'

            bool bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, mapListFrame.getWidth(), iHeightPixels);

            int textColor;

            // render row as 'pressed' (selected)
            if (i == iSkirmishMap) {
                textColor = bHover ? colorDarkerYellow : colorYellow;
                GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrame.getWidth(), iHeightPixels);
            } else {
                textColor = bHover ? colorRed : colorWhite;
                if (bHover) {
                    if (mouse->isLeftButtonClicked()) {
                        GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrame.getWidth(), iHeightPixels);
                        game.iSkirmishMap = i;
                        bool bigMap = PreviewMap[i].height > 64 || PreviewMap[i].width > 64;
//                    spawnWorms = bigMap ? 4 : 2;

                        if (i == 0) {
//                            bDoRandomMap = true;
                        }
                    }
                }
            }


            textDrawer.drawText(mapListFrame.getX() + 4, iDrawY+4, textColor, PreviewMap[i].name);
        }
    }
}
