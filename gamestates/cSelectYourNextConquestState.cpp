#include "d2tmh.h"
#include "cSelectYourNextConquestState.h"


cSelectYourNextConquestState::cSelectYourNextConquestState(cGame &theGame) : cGameState(theGame) {
    state = eRegionState::INTRODUCTION;
    iRegionScene = 0;       // scene for introduction
    iRegionSceneAlpha = 0;  // alpha for scene in introduction state

    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));
}

cSelectYourNextConquestState::~cSelectYourNextConquestState() {

}

void cSelectYourNextConquestState::think() {

}

void cSelectYourNextConquestState::draw() {
    mouse_tile = MOUSE_NORMAL; // global state of mouse

    if (game.iFadeAction == 1) // fading out
    {
        draw_sprite(bmp_screen, bmp_fadeout, 0, 0);
        return;
    }

    if (game.iAlphaScreen == 0) {
        game.iFadeAction = 2;
    }
    // -----------------

    // STEPS:
    // 1. Show current conquered regions
    // 2. Show next progress + story (in message bar)
    // 3. Click next region
    // 4. Set up region and go to GAME_BRIEFING, which will do the rest...-> fade out

    int iHouse = player[0].getHouse();
    int iMission = game.iMission;

    select_palette(player[0].pal);

    if (iRegionSceneAlpha > 255)
        iRegionSceneAlpha = 255;

    if (state == eRegionState::INTRODUCTION) {
        drawStateIntroduction(iHouse, iMission);
    } else if (state == eRegionState::CONQUER_REGIONS) {
        drawStateConquerRegions();
    } else if (state == eRegionState::SELECT_NEXT_CONQUEST) {
        drawStateSelectYourNextConquest(iMission);
    }
    // Draw this last
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[BMP_NEXTCONQ].dat, 0, 0); // title "Select your next Conquest"
    drawLogoInFourCorners(iHouse);

    // draw message
    drawManager->getMessageDrawer()->draw();
    drawManager->drawMouse();

    vsync();
}

void cSelectYourNextConquestState::drawLogoInFourCorners(int iHouse) const {
    int iLogo = -1;

    // Draw your logo
    if (iHouse == ATREIDES)
        iLogo = BMP_NCATR;

    if (iHouse == ORDOS)
        iLogo = BMP_NCORD;

    if (iHouse == HARKONNEN)
        iLogo = BMP_NCHAR;

    // Draw 4 times the logo (in each corner)
    if (iLogo > -1) {
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, 0, 0);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, (640) - 64, 0);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, 0, (480) - 64);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, (640) - 64, (480) - 64);
    }
}

void cSelectYourNextConquestState::drawStateIntroduction(int iHouse,
                                                         int iMission) {// depending on the mission, we tell the story
    if (iRegionScene == 0) {
        REGION_SETUP(iMission, iHouse);
        iRegionScene++;
        drawManager->getMessageDrawer()->setMessage("3 Houses have come to Dune.");
        iRegionSceneAlpha = -5;
    } else if (iRegionScene == 1) {
        // draw the
        set_trans_blender(0, 0, 0, iRegionSceneAlpha);
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0, 12);
        char *cMessage = drawManager->getMessageDrawer()->getMessage();
        if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255) {
            drawManager->getMessageDrawer()->setMessage("To take control of the land.");
            iRegionScene++;
            iRegionSceneAlpha = -5;
        }
    } else if (iRegionScene == 2) {
        draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, 0, 12);
        set_trans_blender(0, 0, 0, iRegionSceneAlpha);
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
        char *cMessage = drawManager->getMessageDrawer()->getMessage();
        if (cMessage[0] == '\0' && iRegionSceneAlpha >= 255) {
            drawManager->getMessageDrawer()->setMessage("That has become divided.");
            iRegionScene++;
            iRegionSceneAlpha = -5;
        }
    } else if (iRegionScene == 3) {
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
        set_trans_blender(0, 0, 0, iRegionSceneAlpha);
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

        if (iRegionSceneAlpha >= 255) {
            iRegionScene = 4;
            conquerRegions();
        }
    }

    if (iRegionSceneAlpha < 255) {
        iRegionSceneAlpha += 5;
    }

    // when  mission is 1, do the '3 houses has come to dune, blah blah story)
}

void cSelectYourNextConquestState::drawStateConquerRegions() {// draw dune first
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

    // draw here stuff
    for (int i = 0; i < 27; i++)
        REGION_DRAW(world[i]);

    // Animate here (so add regions that are conquered)
    char *cMessage = drawManager->getMessageDrawer()->getMessage();

    bool bDone = true;
    for (int i = 0; i < MAX_REGIONS; i++) {
        if (iRegionConquer[i] < 0) continue;

        int iRegNr = iRegionConquer[i];

        if (iRegionHouse[i] > -1) {
            // when the region is NOT this house, turn it into this house
            cRegion &region = world[iRegNr];

            if (region.iHouse != iRegionHouse[i]) {

                if ((cRegionText[i][0] != '\0' && cMessage[0] == '\0') ||
                    (cRegionText[i][0] == '\0')) {

                    // set this up
                    region.iHouse = iRegionHouse[i];
                    region.iAlpha = 1;

                    if (cRegionText[i][0] != '\0') {
                        drawManager->getMessageDrawer()->setMessage(cRegionText[i]);
                    }

                    bDone = false;
                    break;

                } else {
                    bDone = false;
                    break;

                }
            } else {
                // house = ok
                if (region.iAlpha >= 255) {
                    // remove from list
                    iRegionConquer[i] = -1;
                    iRegionHouse[i] = -1; //
                    bDone = false;

                    break;
                } else if (region.iAlpha < 255) {
                    bDone = false;
                    break; // not done yet, so wait before we do another region!
                }
            }
        }
    }

    if (bDone && cMessage[0] == '\0') {
        state = SELECT_NEXT_CONQUEST;
        drawManager->getMessageDrawer()->setMessage("Select your next region.");
    }
}

void cSelectYourNextConquestState::drawStateSelectYourNextConquest(int iMission) {
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, 16, 73);
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, 16, 73);

    // draw here stuff
    for (int i = 0; i < 27; i++)
        REGION_DRAW(world[i]);

    int r = REGION_OVER();

    bool bClickable = false;

    if (r > -1) {
        if (world[r].bSelectable) {
            world[r].iAlpha = 255;
            mouse_tile = MOUSE_ATTACK;
            bClickable = true;
        }
    }

    if (cMouse::isLeftButtonClicked() && bClickable) {
        // selected....
        int iReg = 0;
        for (int ir = 0; ir < MAX_REGIONS; ir++) {
            if (world[ir].bSelectable) {
                if (ir != r) {
                    iReg++;
                } else {
                    break;
                }
            }
        }


        // Calculate mission from region:
        // region 1 = mission 1
        // region 2, 3, 4 = mission 2
        // region 5, 6, 7 = mission 3
        // region 8, 9, 10 = mission 4
        // region 11,12,13 = mission 5
        // region 14,15,16 = mission 6
        // region 17,18,19 = mission 7
        // region 20,21    = mission 8
        // region 22 = mission 9

        // calculate region stuff, and add it up
        int iNewReg = 0;
        if (iMission == 0) iNewReg = 1;
        if (iMission == 1) iNewReg = 2;
        if (iMission == 2) iNewReg = 5;
        if (iMission == 3) iNewReg = 8;
        if (iMission == 4) iNewReg = 11;
        if (iMission == 5) iNewReg = 14;
        if (iMission == 6) iNewReg = 17;
        if (iMission == 7) iNewReg = 20;
        if (iMission == 8) iNewReg = 22;

        iNewReg += iReg;

        //char msg[255];
        //sprintf(msg, "Mission = %d", game.iMission);
        //allegro_message(msg);

        game.mission_init();
        game.setState(GAME_BRIEFING);
        game.iRegion = iNewReg;
        game.iMission++;                        // FINALLY ADD MISSION NUMBER...

        // set up stateMentat
        game.createAndPrepareMentatForHumanPlayer();

        // load map
        game.loadScenario();

        //sprintf(msg, "Mission = %d", game.iMission);
        //allegro_message(msg);

        playMusicByType(MUSIC_BRIEFING);

        //allegro_message(msg);

        game.FADE_OUT();
    }
}

void cSelectYourNextConquestState::interact() {

}

void cSelectYourNextConquestState::REGION_SETUP(int iMission, int iHouse) {
    // The first mission, nothing is 'ready', as the pieces gets placed and taken by the houses.
    // Later, after mission 2, the pieces are already taken. Thats what this function takes care off
    // making sure everything is 'there' to go on with. Hard-coded stuff.

    // First step:
    // remove all pieces (house properties)
    for (int i = 0; i < MAX_REGIONS; i++)
        world[i].bSelectable = false;

    // clear conquer stuff
    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));

    // Per mission assign:
    // Every house has a different campaign, so...

    INI_Load_Regionfile(iHouse, iMission);

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        player[i].init(i);
        player[i].setHouse(i);
    }

    return;
}

void cSelectYourNextConquestState::REGION_DRAW(cRegion &regionPiece) {
    if (regionPiece.iAlpha <= 0) {
        // no alpha, no use in drawing
        return;
    }

    int screenBitDepth = bitmap_color_depth(bmp_screen);

    // HACK HACK - Using a temp player variable, we do a trick to calculate the proper palette for this
    // highly not efficient.... but will do for now
    if (regionPiece.iHouse > -1) {
        cPlayer &temp = player[regionPiece.iHouse];
        select_palette(temp.pal);           // retrieve pal


        // alpha is lower then 255
        // TODO: Move this stuff to timer / think logic
        if (regionPiece.iAlpha < 255) {
            regionPiece.iAlpha += 7;

            if (state == eRegionState::CONQUER_REGIONS) {
                // speed up when holding mouse button
                if (MOUSE_BTN_LEFT()) {
                    regionPiece.iAlpha += 25;
                }
            }
        }

        // When alpha is 255 or higher, do not use trans_sprite, which is useless
        if (regionPiece.iAlpha >= 255) {
            draw_sprite(bmp_screen, (BITMAP *) gfxworld[regionPiece.iTile].dat, regionPiece.x, regionPiece.y);
        } else {
            // TODO: Move this so we only create 16 bit bitmaps once

            // Draw region with trans_sprite, the trick is that we have to convert it to 16 bit first.
            set_trans_blender(0, 0, 0, regionPiece.iAlpha);                // set blender
            BITMAP *tempregion = create_bitmap_ex(screenBitDepth, 256, 256);                // 16 bit bmp
            clear_to_color(tempregion, makecol(255, 0, 255));            // clear
            draw_sprite(tempregion, (BITMAP *) gfxworld[regionPiece.iTile].dat, 0, 0); // copy
            draw_trans_sprite(bmp_screen, tempregion, regionPiece.x, regionPiece.y); //	draw trans
            destroy_bitmap(tempregion); // destroy temp
        }
    } // House > -1

    // select your next conquest... always draw them in the human playing house color
    if (regionPiece.bSelectable && state == SELECT_NEXT_CONQUEST) {
        int iHouse = player[HUMAN].getHouse();
        cPlayer &temp = player[iHouse];
        select_palette(temp.pal);           // retrieve pal

        // House < 0
        if (regionPiece.iHouse < 0) {
            if (regionPiece.iAlpha < 255)
                regionPiece.iAlpha += 5;
        }

        // Alpha >= 255
        if (regionPiece.iAlpha >= 255) {
            draw_sprite(bmp_screen, (BITMAP *) gfxworld[regionPiece.iTile].dat, regionPiece.x, regionPiece.y);
            regionPiece.iAlpha = 1;
        } else {
            // TODO: Move this so we only create 16 bit bitmaps once
            set_trans_blender(0, 0, 0, regionPiece.iAlpha);
            BITMAP *tempregion = create_bitmap_ex(screenBitDepth, 256, 256);
            clear_to_color(tempregion, makecol(255, 0, 255));
            draw_sprite(tempregion, (BITMAP *) gfxworld[regionPiece.iTile].dat, 0, 0);
            draw_trans_sprite(bmp_screen, tempregion, regionPiece.x, regionPiece.y);
            destroy_bitmap(tempregion);
        }
    }
} // End of function

int cSelectYourNextConquestState::REGION_OVER() {
    // when mouse is not even on the map, return -1
    if (mouse_y < 72 || mouse_y > 313 || mouse_x < 16 || mouse_x > 624)
        return -1;

    // from here, we are on a region
    int iRegion = -1;

    // temp bitmap to read from
    BITMAP *tempreg = create_bitmap_ex(8, 640, 480); // 8 bit bitmap
    select_palette(general_palette); // default palette
    clear(tempreg); // clear bitmap

    draw_sprite(tempreg, (BITMAP *) gfxworld[WORLD_DUNE_CLICK].dat, 16, 73);

    int c = getpixel(tempreg, mouse_x, mouse_y);

    //alfont_textprintf(bmp_screen, bene_font, 17,17, makecol(0,0,0), "region %d", c-1);

    iRegion = c - 1;
    destroy_bitmap(tempreg);
    return iRegion;
}

void cSelectYourNextConquestState::REGION_NEW(int x, int y, int iAlpha, int iHouse, int iTile) {
    int iNew = -1;

    for (int i = 0; i < MAX_REGIONS; i++) {
        if (world[i].x < 0 || world[i].y < 0) {
            iNew = i;
            break;
        }
    }

    // invalid index
    if (iNew < 0)
        return;

    world[iNew].x = x;
    world[iNew].y = y;
    world[iNew].iAlpha = iAlpha;
    world[iNew].iHouse = iHouse;
    world[iNew].iTile = iTile;
}

void cSelectYourNextConquestState::INSTALL_WORLD() {
    // create regions
    for (int i = 0; i < MAX_REGIONS; i++) {
        world[i].bSelectable = false;
        world[i].iAlpha = 0;
        world[i].iHouse = -1;
        world[i].iTile = -1;
        world[i].x = -1;
        world[i].y = -1;
    }

    // Now create the regions (x,y wise)

    // FIRST ROW (EASY, SAME Y AXIS)
    REGION_NEW(16, 73, 1, -1, PIECE_DUNE_001);
    REGION_NEW(126, 73, 1, -1, PIECE_DUNE_002);
    REGION_NEW(210, 73, 1, -1, PIECE_DUNE_003);
    REGION_NEW(306, 73, 1, -1, PIECE_DUNE_004);
    REGION_NEW(438, 73, 1, -1, PIECE_DUNE_005);
    REGION_NEW(510, 73, 1, -1, PIECE_DUNE_006);

    // SECOND ROW, HARDER FROM NOW ON (DIFFERENT Y AXIS ALL THE TIME)
    REGION_NEW(16, 91, 1, -1, PIECE_DUNE_007);
    REGION_NEW(158, 149, 1, -1, PIECE_DUNE_008);
    REGION_NEW(282, 135, 1, -1, PIECE_DUNE_009);
    REGION_NEW(362, 105, 1, -1, PIECE_DUNE_010);
    REGION_NEW(456, 117, 1, -1, PIECE_DUNE_011);
    REGION_NEW(544, 105, 1, -1, PIECE_DUNE_012);

    // THIRD ROW
    REGION_NEW(16, 155, 1, -1, PIECE_DUNE_013);
    REGION_NEW(58, 165, 1, -1, PIECE_DUNE_014);
    REGION_NEW(190, 213, 1, -1, PIECE_DUNE_015);
    REGION_NEW(312, 163, 1, -1, PIECE_DUNE_016);
    REGION_NEW(388, 163, 1, -1, PIECE_DUNE_017);
    REGION_NEW(502, 167, 1, -1, PIECE_DUNE_018);
    REGION_NEW(576, 167, 1, -1, PIECE_DUNE_019);

    // FOURTH ROW
    REGION_NEW(16, 237, 1, -1, PIECE_DUNE_020);
    REGION_NEW(62, 255, 1, -1, PIECE_DUNE_021);
    REGION_NEW(134, 245, 1, -1, PIECE_DUNE_022);
    REGION_NEW(282, 257, 1, -1, PIECE_DUNE_023);
    REGION_NEW(360, 253, 1, -1, PIECE_DUNE_024);
    REGION_NEW(406, 213, 1, -1, PIECE_DUNE_025);
    REGION_NEW(448, 269, 1, -1, PIECE_DUNE_026);
    REGION_NEW(514, 227, 1, -1, PIECE_DUNE_027);
}

eGameStateType cSelectYourNextConquestState::getType() {
    return SELECT_YOUR_NEXT_CONQUEST;
}

void cSelectYourNextConquestState::conquerRegions() {
    state = CONQUER_REGIONS;
}
