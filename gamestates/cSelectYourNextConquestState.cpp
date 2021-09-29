#include "d2tmh.h"
#include "cSelectYourNextConquestState.h"


cSelectYourNextConquestState::cSelectYourNextConquestState(cGame &theGame) : cGameState(theGame) {
    state = eRegionState::REGSTATE_INIT;
    regionSceneState = eRegionSceneState::SCENE_INIT;

    iRegionSceneAlpha = 0;  // alpha for scene in introduction state

    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));

    calculateOffset();

    regionClickMapBmp = nullptr;
}

void cSelectYourNextConquestState::calculateOffset() {
    offsetX = (game.screen_x - 640) / 2;
    offsetY = (game.screen_y - 480) / 2; // same goes for offsetY (but then for 480 height).

}

cSelectYourNextConquestState::~cSelectYourNextConquestState() {
    destroy_bitmap(regionClickMapBmp);
}

void cSelectYourNextConquestState::think() {

    // First time INIT
    if (state == eRegionState::REGSTATE_INIT) {
        // temp bitmap to read from
        regionClickMapBmp = create_bitmap_ex(8, 640, 480); // 8 bit bitmap
        select_palette(general_palette); // default palette
        clear(regionClickMapBmp); // clear bitmap

        // NOTE: No need to use Offset here, as it is on a tempreg and we pretend our mouse is on that BMP as well
        // we substract the offset from mouse coordinates to compensate
        draw_sprite(regionClickMapBmp, (BITMAP *) gfxworld[WORLD_DUNE_CLICK].dat, 16, 73);

        state = eRegionState::REGSTATE_INTRODUCTION;
        return;
    }

    if (state == eRegionState::REGSTATE_INTRODUCTION) {
        if (iRegionSceneAlpha < 255) {
            iRegionSceneAlpha += 1;
        }

        if (iRegionSceneAlpha > 255) {
            iRegionSceneAlpha = 255;
        }

        int iHouse = players[0].getHouse();
        int iMission = game.iMission;

        if (regionSceneState == SCENE_INIT) {
            REGION_SETUP_NEXT_MISSION(iMission, iHouse);
            drawManager->getMessageDrawer()->setMessage("3 Houses have come to Dune.");
            transitionToNextRegionSceneState(SCENE_THREE_HOUSES_COME_FOR_DUNE);
        } else if (regionSceneState == SCENE_THREE_HOUSES_COME_FOR_DUNE) {
            bool hasMessage = drawManager->getMessageDrawer()->hasMessage();
            if (!hasMessage && iRegionSceneAlpha >= 255) {
                drawManager->getMessageDrawer()->setMessage("To take control of the land.");
                transitionToNextRegionSceneState(SCENE_TO_TAKE_CONTROL_OF_THE_LAND);
            }
        } else if (regionSceneState == SCENE_TO_TAKE_CONTROL_OF_THE_LAND) {
            bool hasMessage = drawManager->getMessageDrawer()->hasMessage();
            if (!hasMessage && iRegionSceneAlpha >= 255) {
                drawManager->getMessageDrawer()->setMessage("That has become divided.");
                transitionToNextRegionSceneState(SCENE_THAT_HAS_BECOME_DIVIDED);
            }
        } else if (regionSceneState == SCENE_THAT_HAS_BECOME_DIVIDED) {
            if (iRegionSceneAlpha >= 255) {
                transitionToNextRegionSceneState(SCENE_SELECT_YOUR_NEXT_CONQUEST);
                conquerRegions(); // this will change the message bar and state
            }
        }
    }

    if (state == eRegionState::REGSTATE_CONQUER_REGIONS ||
        state == eRegionState::REGSTATE_INTRODUCTION) {
        for (int i = 0; i < 27; i++) {
            cRegion &regionPiece = world[i];

            if (regionPiece.iHouse < 0) continue; // skip pieces that do not belong to any house

            if (regionPiece.iAlpha < 255) {
                regionPiece.iAlpha += 3;

                // speed up when holding mouse button
                if (game.getMouse()->isLeftButtonPressed()) {
                    regionPiece.iAlpha += 3;
                }
            }
        }
    }

    // select your next conquest... always draw them in the human playing house color
    if (state == REGSTATE_SELECT_NEXT_CONQUEST) {
        if (selectNextConquestAlpha < 255) {
            selectNextConquestAlpha += 1;
        } else {
            selectNextConquestAlpha = 1;
        }

        for (int i = 0; i < 27; i++) {
            cRegion &regionPiece = world[i];

            if (!regionPiece.bSelectable) continue; // only animate selectable pieces

            regionPiece.iAlpha = selectNextConquestAlpha;
        }
    }

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
    int veryDark = makecol(48, 28, 0);
    clear_to_color(bmp_screen, veryDark);

    // STEPS:
    // 1. Show current conquered regions
    // 2. Show next progress + story (in message bar)
    // 3. Click next region
    // 4. Set up region and go to GAME_BRIEFING, which will do the rest...-> fade out

    int iHouse = players[0].getHouse();

    PALETTE &humanPlayerPalette = players[0].pal;
    select_palette(humanPlayerPalette);

    if (state == eRegionState::REGSTATE_INTRODUCTION) {
        drawStateIntroduction();
    } else if (state == eRegionState::REGSTATE_CONQUER_REGIONS) {
        drawStateConquerRegions();
    } else if (state == eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) {
        drawStateSelectYourNextConquest();
    } else if (state == eRegionState::REGSTATE_FADEOUT) {
        drawStateConquerRegions();
    }

    // make sure to select the correct palette again
    select_palette(humanPlayerPalette);

    // Draw this last
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[BMP_NEXTCONQ].dat, offsetX, offsetY); // title "Select your next Conquest"
    drawLogoInFourCorners(iHouse);
    drawManager->getMessageDrawer()->draw();

    drawManager->drawMouse();
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
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, offsetX, offsetY);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, offsetX + (640) - 64, offsetY);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, offsetX, offsetY + (480) - 64);
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[iLogo].dat, offsetX + (640) - 64, offsetY + (480) - 64);
    }
}

void cSelectYourNextConquestState::drawStateIntroduction() {
    if (regionSceneState == SCENE_THREE_HOUSES_COME_FOR_DUNE) {
        allegroDrawer->setTransBlender(0, 0, 0, iRegionSceneAlpha);

        // draw dune planet (being faded in)
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, offsetX, offsetY + 12);
    } else if (regionSceneState == SCENE_TO_TAKE_CONTROL_OF_THE_LAND) {
        draw_sprite(bmp_screen, (BITMAP *) gfxinter[BMP_GAME_DUNE].dat, offsetX, offsetY + 12); // dune is opaque
        allegroDrawer->setTransBlender(0, 0, 0, iRegionSceneAlpha);
        // draw dune world map over Dune planet , transitioning
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, offsetX + 16, offsetY + 73);
    } else if (regionSceneState == SCENE_THAT_HAS_BECOME_DIVIDED) {
        // now the world map is opaque
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, offsetX + 16, offsetY + 73);
        allegroDrawer->setTransBlender(0, 0, 0, iRegionSceneAlpha);

        // introduce the borders (world pieces), draw over world dune, transitioning
        draw_trans_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, offsetX + 16, offsetY + 73);
    } else if (regionSceneState == SCENE_SELECT_YOUR_NEXT_CONQUEST) {
        draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, offsetX + 16, offsetY + 73);
    }
}

void cSelectYourNextConquestState::drawStateConquerRegions() { // draw dune first
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, offsetX + 16, offsetY + 73);
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, offsetX + 16, offsetY + 73);

    // draw here stuff
    for (int i = 0; i < 27; i++) {
        REGION_DRAW(world[i]);
    }

    // Animate here (so add regions that are conquered)
    char *cMessage = drawManager->getMessageDrawer()->getMessage();

    bool isFinishedConqueringRegions = true;
    bool isDisplayingMessage = cMessage[0] == '\0';

    for (int i = 0; i < MAX_REGIONS; i++) {
        if (iRegionConquer[i] < 0) continue;
        int houseThatConquersTheRegion = iRegionHouse[i];
        if (houseThatConquersTheRegion < 0) continue;

        int iRegNr = iRegionConquer[i];

        cRegion &region = world[iRegNr];

        // when the region is NOT this house, turn it into this house
        if (region.iHouse != houseThatConquersTheRegion) {
            char *regionTextString = cRegionText[i];
            bool isRegionTextGiven = regionTextString[0] != '\0';
            bool isRegionTextEmpty = regionTextString[0] == '\0';

            if ((isRegionTextGiven && isDisplayingMessage) ||
                isRegionTextEmpty) {
                // set this up
                region.iHouse = houseThatConquersTheRegion;
                region.iAlpha = 1; // this makes it > 0 and thus it will become opaque over time (see THINK function)

                if (isRegionTextGiven) {
                    drawManager->getMessageDrawer()->setMessage(regionTextString);
                }

                isFinishedConqueringRegions = false;
                break;
            } else {
                isFinishedConqueringRegions = false;
                break;

            }
        } else {
            // house = the same
            if (region.iAlpha >= 255) {
                // remove this from the 'regionToConquer' index
                iRegionConquer[i] = -1;
                houseThatConquersTheRegion = -1; //
                isFinishedConqueringRegions = false;

                break;
            } else if (region.iAlpha < 255) {
                isFinishedConqueringRegions = false;
                break; // not done yet, so wait before we do another region!
            }
        }
    }

    if (isFinishedConqueringRegions && isDisplayingMessage) {
        state = REGSTATE_SELECT_NEXT_CONQUEST;
        drawManager->getMessageDrawer()->setMessage("Select your next region.");
        drawManager->getMessageDrawer()->setKeepMessage(true);
    }
}

void cSelectYourNextConquestState::drawStateSelectYourNextConquest() {
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE].dat, offsetX + 16, offsetY + 73);
    draw_sprite(bmp_screen, (BITMAP *) gfxworld[WORLD_DUNE_REGIONS].dat, offsetX + 16, offsetY + 73);

    int regionMouseIsHoveringOver = REGION_OVER();

    if (regionMouseIsHoveringOver > -1) {
        cRegion &region = world[regionMouseIsHoveringOver];
        if (region.bSelectable) {
            region.iAlpha = 256;
            mouse_tile = MOUSE_ATTACK;
        }
    }

    // draw here stuff
    for (int i = 0; i < 27; i++) {
        REGION_DRAW(world[i]);
    }
}

void cSelectYourNextConquestState::interact() {
    // no interaction unless we select next conquest
    if (state != eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) return;

    int iMission = game.iMission;

    int regionMouseIsHoveringOver = REGION_OVER();

    bool bClickable = false;

    if (regionMouseIsHoveringOver > -1) {
        cRegion &region = world[regionMouseIsHoveringOver];
        if (region.bSelectable) {
            region.iAlpha = 255;
            mouse_tile = MOUSE_ATTACK;
            bClickable = true;
        }
    }

    if (game.getMouse()->isLeftButtonClicked() && bClickable) {
        // selected....
        int iReg = 0;
        for (int ir = 0; ir < MAX_REGIONS; ir++) {
            if (world[ir].bSelectable) {
                if (ir != regionMouseIsHoveringOver) {
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

        state = REGSTATE_FADEOUT;
        game.FADE_OUT();
    }
}

void cSelectYourNextConquestState::REGION_SETUP_LOST_MISSION(int iMission, int iHouse) {
    // The first mission, nothing is 'ready', as the pieces gets placed and taken by the houses.
    // Later, after mission 2, the pieces are already taken. That's what this function takes care off
    // making sure everything is 'there' to go on with. Hard-coded stuff.
    drawManager->getMessageDrawer()->initRegionPosition(offsetX, offsetY);

    selectNextConquestAlpha = 1;

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        players[i].init(i, nullptr);
        players[i].setHouse(i);
    }

    return;
}

void cSelectYourNextConquestState::REGION_SETUP_NEXT_MISSION(int iMission, int iHouse) {
    // The first mission, nothing is 'ready', as the pieces gets placed and taken by the houses.
    // Later, after mission 2, the pieces are already taken. That's what this function takes care off
    // making sure everything is 'there' to go on with. Hard-coded stuff.
    drawManager->getMessageDrawer()->initRegionPosition(offsetX, offsetY);

    // make world pieces not selectable
    for (int i = 0; i < MAX_REGIONS; i++) {
        world[i].bSelectable = false;
    }

    // clear conquer stuff
    memset(iRegionConquer, -1, sizeof(iRegionConquer));
    memset(iRegionHouse, -1, sizeof(iRegionHouse));
    memset(cRegionText, 0, sizeof(cRegionText));

    // Per mission assign:
    // Every house has a different campaign, so...

    INI_Load_Regionfile(iHouse, iMission);

    selectNextConquestAlpha = 1;

    // prepare players, so we know house index == player index (for colorizing region pieces)
    for (int i = 1; i < FREMEN; i++) {
        players[i].init(i, nullptr);
        players[i].setHouse(i);
    }

    return;
}

void cSelectYourNextConquestState::REGION_DRAW(cRegion &regionPiece) {
    if (regionPiece.iAlpha <= 0) {
        // no alpha, no use in drawing
        return;
    }

    // HACK HACK - Using a temp player variable, we do a trick to calculate the proper palette for this
    // highly not efficient.... but will do for now
    if (regionPiece.iHouse > -1) {
        // single player campaign has house ID == player ID, so we can do this hack and assume player with iHouse
        // is the player we want to get the correct house collor for this piece...
        cPlayer &temp = players[regionPiece.iHouse];
        select_palette(temp.pal);
        drawRegion(regionPiece);
    } // House > -1

    // select your next conquest... always draw them in the human playing house color
    if (regionPiece.bSelectable && state == eRegionState::REGSTATE_SELECT_NEXT_CONQUEST) {
        int iHouse = players[HUMAN].getHouse();
        cPlayer &temp = players[iHouse];
        select_palette(temp.pal);
        drawRegion(regionPiece);
    }

}

void cSelectYourNextConquestState::drawRegion(cRegion &regionPiece) const {
    int regionX = offsetX + regionPiece.x;
    int regionY = offsetY + regionPiece.y;

    if (regionPiece.iAlpha >= 255) {
        draw_sprite(bmp_screen, regionPiece.bmp, regionX, regionY);
    } else {
        allegroDrawer->setTransBlender(0, 0, 0, regionPiece.iAlpha);
        clear_to_color(regionPiece.bmpHighBit, makecol(255, 0, 255));
        draw_sprite(regionPiece.bmpHighBit, regionPiece.bmp, 0, 0);
        draw_trans_sprite(bmp_screen, regionPiece.bmpHighBit, regionX, regionY);
    }
}
// End of function

int cSelectYourNextConquestState::REGION_OVER() {
    int mouseX = mouse_x;
    int mouseY = mouse_y;

    // when mouse is not even on the map, return -1
    cRectangle mapRect(offsetX + 16, offsetY + 72, 608, 241);
    if (!mapRect.isMouseOver(mouseX, mouseY)) return -1;

    // from here, we are on a region

    int c = getpixel(regionClickMapBmp, (mouseX-offsetX), (mouseY-offsetY));

    //alfont_textprintf(bmp_screen, bene_font, 17,17, makecol(0,0,0), "region %d", c-1);

    return c - 1;
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

    cRegion &region = world[iNew];
    region.x = x;
    region.y = y;
    region.iAlpha = iAlpha;
    region.iHouse = iHouse;
    region.iTile = iTile;
    region.bmp = (BITMAP *) gfxworld[iTile].dat;

    int screenBitDepth = bitmap_color_depth(bmp_screen);
    BITMAP *tempregion = create_bitmap_ex(screenBitDepth, region.bmp->w, region.bmp->h);
    clear_to_color(tempregion, makecol(255, 0, 255));
    region.bmpHighBit = tempregion;
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
        world[i].bmp = nullptr;
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
    return GAMESTATE_SELECT_YOUR_NEXT_CONQUEST;
}

void cSelectYourNextConquestState::conquerRegions() {
    state = REGSTATE_CONQUER_REGIONS;
}

void cSelectYourNextConquestState::transitionToNextRegionSceneState(eRegionSceneState newSceneState) {
    regionSceneState = newSceneState;
    iRegionSceneAlpha = 0;
}

void cSelectYourNextConquestState::destroy() {
    for (int i = 0; i < 27; i++) {
        cRegion &region = world[i];
        if (region.bmpHighBit) {
            destroy_bitmap(region.bmpHighBit);
        }
    }
}
