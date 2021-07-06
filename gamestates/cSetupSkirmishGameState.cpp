#include <algorithm>
#include "d2tmh.h"


cSetupSkirmishGameState::cSetupSkirmishGameState(cGame &theGame) : cGameState(theGame) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        // index 0 == human player, but to keep our lives sane we don't change the index.

        // player 0 (HUMAN) is always playing,
        // and 1 additional AI is minimally required to play
        skirmishPlayer[i].bPlaying = (i <= 1);

        // just some defaults
        skirmishPlayer[i].iUnits = 3;
        skirmishPlayer[i].iHouse = 0; // random house
    }
    textDrawer = cTextDrawer(bene_font);

    mouse = game.getMouse();
}

cSetupSkirmishGameState::~cSetupSkirmishGameState() {
}

void cSetupSkirmishGameState::think() {

}

void cSetupSkirmishGameState::draw() {
    // FADING STUFF
    if (game.iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (game.iAlphaScreen == 0)
        game.iFadeAction = 2;
    // -----------------

    int screen_x = game.screen_x;
    int screen_y = game.screen_y;

    int darkishBackgroundColor = makecol(32, 32, 32);
    int darkishBorderColor = makecol(227, 229, 211);
    int yellow = makecol(255, 207, 41);

    bool bFadeOut=false;

    draw_sprite(bmp_screen,(BITMAP *)gfxinter[BMP_GAME_DUNE].dat, game.screen_x * 0.2, (game.screen_y * 0.5));

    for (int dy=0; dy < game.screen_y; dy+=2) {
        line(bmp_screen, 0, dy, screen_x, dy, makecol(0,0,0));
    }

    int topBarWidth = screen_x + 4;
    int topBarHeight = 21;
    int previewMapHeight = 129;
    int previewMapWidth = 129;

    int sidebarWidth = 158;

    // title box
    GUI_DRAW_FRAME(-1, -1, topBarWidth, topBarHeight);

    int creditsX = (screen_x / 2) - (alfont_text_length(bene_font, "Skirmish") / 2);
    GUI_DRAW_BENE_TEXT(creditsX, 1, "Skirmish");

    int topRightBoxWidth = 276;

    // Players title bar
    int playerTitleBarWidth = screen_x - topRightBoxWidth;
    int playerTitleBarHeight = topBarHeight;
    int playerTitleBarX = 0;
    int playerTitleBarY = topBarHeight;
    GUI_DRAW_FRAME_WITH_COLORS(playerTitleBarX, playerTitleBarY, playerTitleBarWidth, playerTitleBarHeight, makecol(255, 255, 255), darkishBackgroundColor );

    // this is the box at the right from the Player list
    int topRightBoxHeight = playerTitleBarHeight + previewMapHeight;
    int topRightBoxX = screen_x - topRightBoxWidth;
    int topRightBoxY = topBarHeight;
    GUI_DRAW_FRAME(topRightBoxX, topRightBoxY, topRightBoxWidth, topRightBoxHeight);

    // player list
    int playerListWidth = playerTitleBarWidth;
    int playerListBarHeight = topRightBoxHeight;
    int playerListBarX = 0;
    int playerListBarY = playerTitleBarY + topBarHeight;
    GUI_DRAW_FRAME_WITH_COLORS(playerListBarX, playerListBarY, playerListWidth, playerListBarHeight, makecol(255, 255, 255), darkishBackgroundColor);

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
    GUI_DRAW_FRAME_WITH_COLORS(mapListTopX, mapListTopY, mapListWidth, mapListHeight, darkishBorderColor, darkishBackgroundColor);

    int previewMapY = topBarHeight + 6;
    int previewMapX = screen_x - (previewMapWidth + 6);

    // TITLE: Map list
    GUI_DRAW_FRAME_WITH_COLORS(mapListFrameX, mapListFrameY, mapListFrameWidth, mapListFrameHeight, darkishBorderColor, darkishBackgroundColor);

    textDrawer.drawTextCentered("Maps", mapListFrameX, mapListFrameWidth, mapListFrameY + 4, yellow);

    int iStartingPoints=0;

    ///////
    // DRAW PREVIEW MAP
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

    int widthOfSomething = 274; //??
    int startPointsX = screen_x - widthOfSomething;
    int startPointsY = previewMapY;
    int startPointHitBoxWidth = 130;
    int startPointHitBoxHeight = 16;

    textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, "Startpoints: %d", iStartingPoints);

    bool bDoRandomMap=false;

    if ((mouse_x >= startPointsX && mouse_x <= (startPointsX + startPointHitBoxWidth)) && (mouse_y >= startPointsY && mouse_y <= (startPointsY + startPointHitBoxHeight)))
    {
        textDrawer.drawTextWithOneInteger(startPointsX, startPointsY, makecol(255, 0, 0), "Startpoints: %d", iStartingPoints);

        if (mouse->isLeftButtonClicked())
        {
            game.iSkirmishStartPoints++;

            if (game.iSkirmishStartPoints > 4) {
                game.iSkirmishStartPoints = 2;
            }

            bDoRandomMap=true;
        }

        if (mouse->isRightButtonClicked())
        {
            game.iSkirmishStartPoints--;

            if (game.iSkirmishStartPoints < 2) {
                game.iSkirmishStartPoints = 4;
            }

            bDoRandomMap=true;
        }
    }

    int const iHeightPixels=topBarHeight;

    int iDrawY=-1;
    int iDrawX=screen_x - widthOfSomething;
    int iEndX=screen_y;
    int iColor=makecol(255,255,255);

    // yes, this means higher resolutions can show more maps.. for now
    int maxMapsInList=std::min((mapListHeight / iHeightPixels), MAX_SKIRMISHMAPS);


    // for every map that we read , draw here
    for (int i=0; i < maxMapsInList; i++) {
        if (PreviewMap[i].name[0] != '\0') {
            iDrawY=mapListFrameY+(i*iHeightPixels)+i+iHeightPixels; // skip 1 bar because the 1st = 'random map'

            bool bHover = GUI_DRAW_FRAME(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);

            iColor=makecol(255,255,255);

            if (bHover)	{
                // Mouse reaction
                iColor=makecol(255,0,0);

                if (mouse->isLeftButtonClicked()) {
                    GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);
                    game.iSkirmishMap = i;

                    if (i == 0) {
                        bDoRandomMap=true;
                    }
                }
            }

            if (i == iSkirmishMap) {
                iColor=yellow;
                if (bHover) {
                    iColor = makecol(225, 177, 21); // a bit darker yellow to give some visual clue
                }
                GUI_DRAW_FRAME_PRESSED(iDrawX, iDrawY, mapListFrameWidth, iHeightPixels);
            }

            textDrawer.drawText(mapListFrameX + 4, iDrawY+4, iColor, PreviewMap[i].name);
        }
    }

    alfont_textprintf(bmp_screen, bene_font, 4, 26, makecol(0,0,0), "Player      House      Credits       Units    Team");
    alfont_textprintf(bmp_screen, bene_font, 4, 25, makecol(255,255,255), "Player      House      Credits       Units    Team");


    bool bHover=false;

    // draw players who will be playing ;)
    for (int p=0; p < (AI_WORM-1); p++)	{
        int iDrawY=playerListBarY + 4 +(p*22);
        if (p < iStartingPoints) {
            // player playing or not
            s_SkirmishPlayer &aiPlayer = skirmishPlayer[p];
            if (p == HUMAN)	{
                alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "Human");
                alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(255,255,255), "Human");
            } else {

                alfont_textprintf(bmp_screen, bene_font, 4,iDrawY+1, makecol(0,0,0), "  CPU");

                // move hovers over... :/
                if ((mouse_x >= 4 && mouse_x <= 73) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16))) {
                    if (aiPlayer.bPlaying) {
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY, makecol(game.getFadeSelect(), 0, 0), "  CPU");
                    } else {
                        // not available
                        alfont_textprintf(bmp_screen, bene_font, 4, iDrawY,
                                          makecol((game.getFadeSelect() / 2), (game.getFadeSelect() / 2), (game.getFadeSelect() / 2)), "  CPU");
                    }

                    if (mouse->isLeftButtonClicked())	{
                        if (aiPlayer.bPlaying) {
                            aiPlayer.bPlaying = false;
                        } else {
                            aiPlayer.bPlaying = true;
                        }
                    }
                }
                else
                {
                    if (aiPlayer.bPlaying)
                        alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(255,255,255), "  CPU");
                    else
                        alfont_textprintf(bmp_screen, bene_font, 4,iDrawY, makecol(128,128,128), "  CPU");
                }
            }

            // HOUSE
            bHover=false;
            char cHouse[30];
            memset(cHouse, 0, sizeof(cHouse));

            cPlayer &cPlayer = players[p];


            if (aiPlayer.iHouse > 0) {
                sprintf(cHouse, cPlayer::getHouseNameForId(aiPlayer.iHouse).c_str());
            } else {
                sprintf(cHouse, "Random");
            }

            alfont_textprintf(bmp_screen, bene_font, 74,iDrawY+1, makecol(0,0,0), "%s", cHouse);

            if ((mouse_x >= 74 && mouse_x <= 150) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
            }
            else
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(255,255,255), "%s", cHouse);
                else
                    alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(128,128,128), "%s", cHouse);

            }

            if (bHover)
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol(game.getFadeSelect(),0,0), "%s", cHouse);
                else
                    alfont_textprintf(bmp_screen, bene_font, 74,iDrawY, makecol((game.getFadeSelect()/2),(game.getFadeSelect()/2),(game.getFadeSelect()/2)), "%s", cHouse);


                if (mouse->isLeftButtonClicked())
                {
                    aiPlayer.iHouse++;

                    // Only human player can be Sardaukar?
                    if (p > 0)
                    {
                        if (aiPlayer.iHouse > SARDAUKAR) {
                            aiPlayer.iHouse = 0;
                        }
                    }
                    else
                    {
                        if (aiPlayer.iHouse > ORDOS) {
                            aiPlayer.iHouse = 0;
                        }
                    }
                }

                if (mouse->isRightButtonClicked())
                {
                    aiPlayer.iHouse--;
                    if (p > 0)
                    {
                        if (aiPlayer.iHouse < 0) {
                            aiPlayer.iHouse = SARDAUKAR;
                        }
                    }
                    else
                    {
                        if (aiPlayer.iHouse < 0) {
                            aiPlayer.iHouse =ORDOS;
                        }
                    }
                }
            }

            // Credits
            bHover=false;

            alfont_textprintf(bmp_screen, bene_font, 174,iDrawY+1, makecol(0,0,0), "%d", (int) cPlayer.getCredits());

            //rect(bmp_screen, 174, iDrawY, 230, iDrawY+16, makecol(255,255,255));

            if ((mouse_x >= 174 && mouse_x <= 230) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(255,255,255), "%d", (int) cPlayer.getCredits());
            }
            else
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(255,255,255), "%d", (int) cPlayer.getCredits());
                else
                    alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(128,128,128), "%d", (int) cPlayer.getCredits());

            }

            if (bHover)
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 174,iDrawY, makecol(game.getFadeSelect(),0,0), "%d", (int) cPlayer.getCredits());
                else
                    alfont_textprintf(bmp_screen, bene_font, 174, iDrawY, makecol((game.getFadeSelect()/2),(game.getFadeSelect()/2),(game.getFadeSelect()/2)), "%d", cPlayer.getCredits());

                if (mouse->isLeftButtonClicked())
                {
                    cPlayer.giveCredits(500);
                    if (cPlayer.getCredits() > 10000) {
                        cPlayer.setCredits(1000);
                    }
                }

                if (mouse->isRightButtonClicked())
                {
                    cPlayer.substractCredits(500);
                    if (cPlayer.getCredits() < 1000) {
                        cPlayer.setCredits(10000);
                    }
                }
            }

            // Units
            bHover = false;

            alfont_textprintf(bmp_screen, bene_font, 269,iDrawY+1, makecol(0,0,0), "%d", aiPlayer.iUnits);

            //rect(bmp_screen, 269, iDrawY, 290, iDrawY+16, makecol(255,255,255));

            if ((mouse_x >= 269 && mouse_x <= 290) && (mouse_y >= iDrawY && mouse_y <= (iDrawY+16)))
                bHover=true;

            if (p == 0)
            {
                alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(255,255,255), "%d", aiPlayer.iUnits);
            }
            else
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(255,255,255), "%d", aiPlayer.iUnits);
                else
                    alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(128,128,128), "%d", aiPlayer.iUnits);

            }

            if (bHover)
            {
                if (aiPlayer.bPlaying)
                    alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol(game.getFadeSelect(),0,0), "%d", aiPlayer.iUnits);
                else
                    alfont_textprintf(bmp_screen, bene_font, 269, iDrawY, makecol((game.getFadeSelect()/2),(game.getFadeSelect()/2),(game.getFadeSelect()/2)), "%d", aiPlayer.iUnits);

                if (mouse->isLeftButtonClicked())
                {
                    aiPlayer.iUnits++;
                    if (aiPlayer.iUnits > 10) {
                        aiPlayer.iUnits = 1;
                    }
                }

                if (mouse->isRightButtonClicked())
                {
                    aiPlayer.iUnits--;
                    if (aiPlayer.iUnits < 1) {
                        aiPlayer.iUnits = 10;
                    }
                }
            }

            // Team
            bHover=false;
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
        randomMapGenerator.generateRandomMap();
    }

    // back
    if (MOUSE_WITHIN_RECT(backButtonX, backButtonY, backButtonWidth, backButtonHeight)) {
        textDrawer.drawTextBottomLeft(makecol(255,0,0), " BACK");
    }

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {
        textDrawer.drawTextBottomRight(makecol(255, 0, 0), "START");
    } // mouse hovers over "START"

    // MOUSE
    draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);

    if (bFadeOut) {
        game.FADE_OUT();
    }
}

void cSetupSkirmishGameState::interact() {
    int iSkirmishMap = game.iSkirmishMap;

    int topBarHeight = 21;
    int screen_y = game.screen_y;
    int screen_x = game.screen_x;

    int backButtonWidth = textDrawer.textLength(" BACK");
    int backButtonHeight = topBarHeight;
    int backButtonY = screen_y - topBarHeight;
    int backButtonX = 0;

    if (MOUSE_WITHIN_RECT(backButtonX, backButtonY, backButtonWidth, backButtonHeight)) {
        if (mouse->isLeftButtonClicked()) {
            game.FADE_OUT();
            game.setState(GAME_MENU); // this deletes the current state object, so has to be last statement!
            return;
        }
    }

    int startButtonWidth = textDrawer.textLength("START");
    int startButtonHeight = topBarHeight;
    int startButtonY = screen_y - topBarHeight;
    int startButtonX = screen_x - startButtonWidth;

    if (MOUSE_WITHIN_RECT(startButtonX, startButtonY, startButtonWidth, startButtonHeight)) {

        // START
        if ((mouse->isLeftButtonClicked() && iSkirmishMap > -1)) {
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

            // set up players and their units
            for (int p = 0; p < MAX_PLAYERS; p++)	{

                s_SkirmishPlayer &sSkirmishPlayer = skirmishPlayer[p];

                int iHouse = sSkirmishPlayer.iHouse; // get house selected, which can be 0 for RANDOM

                // not playing.. do nothing (only for playable factions)
                bool playableFaction = p < AI_CPU5;

                if (playableFaction) {
                    if (!sSkirmishPlayer.bPlaying) continue;

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
                                if (skirmishPlayer[pl].iHouse > 0 &&
                                    skirmishPlayer[pl].iHouse == iHouse) {
                                    houseInUse=true;
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

                cPlayer &cPlayer = players[p];

                // TEAM Logic
                if (p == HUMAN) {
                    cPlayer.setTeam(0);
                } else if (p == AI_CPU5) {
                    cPlayer.setTeam(0);
                } else if (p == AI_CPU6) {
                    cPlayer.setTeam(2); // worm team is against everyone
                } else {
                    // all other AI's are their own team (campaign == AI's are friends, here they are enemies)
                    cPlayer.setTeam(p);
                }

                cPlayer.setHouse(iHouse);

                // from here, ignore non playable factions
                if (!playableFaction) continue;

                cPlayer.setFocusCell(iStartPositions[p]);

                // Set map position
                if (p == HUMAN) {
                    mapCamera->centerAndJumpViewPortToCell(cPlayer.getFocusCell());
                }

                // create constyard
                cAbstractStructure *s = cStructureFactory::getInstance()->createStructure(cPlayer.getFocusCell(), CONSTYARD, p);

                // when failure, create mcv instead
                if (s == NULL) {
                    UNIT_CREATE(cPlayer.getFocusCell(), MCV, p, true);
                }

                // amount of units
                int u=0;

                // create units
                while (u < sSkirmishPlayer.iUnits) {
                    int iX= map.getCellX(cPlayer.getFocusCell());
                    int iY= map.getCellY(cPlayer.getFocusCell());
                    int iType=rnd(12);

                    iX-=4;
                    iY-=4;
                    iX+=rnd(9);
                    iY+=rnd(9);

                    // convert house specific stuff
                    if (cPlayer.getHouse() == ATREIDES) {
                        if (iType == DEVASTATOR || iType == DEVIATOR) {
                            iType = SONICTANK;
                        }

                        if (iType == TROOPERS) {
                            iType = INFANTRY;
                        }

                        if (iType == TROOPER) {
                            iType = SOLDIER;
                        }

                        if (iType == RAIDER) {
                            iType = TRIKE;
                        }
                    }

                    // ordos
                    if (cPlayer.getHouse() == ORDOS) {
                        if (iType == DEVASTATOR || iType == SONICTANK) {
                            iType = DEVIATOR;
                        }

                        if (iType == TRIKE) {
                            iType = RAIDER;
                        }
                    }

                    // harkonnen
                    if (cPlayer.getHouse() == HARKONNEN) {
                        if (iType == DEVIATOR || iType == SONICTANK) {
                            iType = DEVASTATOR;
                        }

                        if (iType == TRIKE || iType == RAIDER) {
                            iType = QUAD;
                        }

                        if (iType == SOLDIER) {
                            iType = TROOPER;
                        }

                        if (iType == INFANTRY) {
                            iType = TROOPERS;
                        }
                    }

                    int cell = map.getCellWithMapBorders(iX, iY);
                    int r = UNIT_CREATE(cell, iType, p, true);
                    if (r > -1)
                    {
                        u++;
                    }
                }

                char msg[255];
                sprintf(msg, "Wants %d amount of units; amount created %d", sSkirmishPlayer.iUnits, u);
                cLogger::getInstance()->log(LOG_TRACE, COMP_SKIRMISHSETUP, "Creating units", msg, OUTC_NONE, p, iHouse);
            }

            game.FADE_OUT();
            playMusicByType(MUSIC_PEACE);

            // TODO: spawn a few worms
            drawManager->getMessageDrawer()->initCombatPosition();

            game.setState(GAME_PLAYING); // this deletes the current state object
            return;
        } // mouse clicks on START (and skirmish map is selected)
    } // mouse hovers over "START"
}

eGameStateType cSetupSkirmishGameState::getType() {
    return GAMESTATE_SETUP_SKIRMISH_GAME;
}