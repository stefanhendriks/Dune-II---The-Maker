#include "cStructureDrawer.h"

#include "drawers/SDLDrawer.hpp"
#include "controls/cGameControlsContext.h"
#include "data/gfxdata.h"
#include "d2tmc.h"
#include "gameobjects/structures/cGunTurret.h"
#include "gameobjects/structures/cRepairFacility.h"
#include "gameobjects/structures/cRocketTurret.h"
#include "gameobjects/structures/cWindTrap.h"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>

void cStructureDrawer::drawStructuresFirstLayer()
{
    drawStructuresForLayer(0);
}

void cStructureDrawer::drawStructuresSecondLayer()
{
    drawStructuresForLayer(2);
}

void cStructureDrawer::drawStructuresHealthBars()
{
    cGameControlsContext *context = players[HUMAN].getGameControlsContext();

    // DRAW HEALTH
    if (context->isMouseOverStructure()) {
        int i = context->getIdOfStructureWhereMouseHovers();
        drawStructureHealthBar(i);
    }
}

void cStructureDrawer::drawRectangleOfStructure(cAbstractStructure *theStructure, SDL_Color color)
{
    assert(theStructure);
    int drawX = theStructure->iDrawX();
    int drawY = theStructure->iDrawY();
    int width = sStructureInfo[theStructure->getType()].bmp_width - 1;
    int height = sStructureInfo[theStructure->getType()].bmp_height - 1;

    int width_x = mapCamera->factorZoomLevel(width);
    int height_y = mapCamera->factorZoomLevel(height);

    //_rect(bmp_screen, drawX, drawY, drawX + width_x, drawY + height_y, color);
    renderDrawer->renderRectFillColor(drawX, drawY, width_x, height_y, color.r, color.g, color.b, 96);
}

void cStructureDrawer::drawStructurePrebuildAnimation(cAbstractStructure *structure)
{
    int iDrawPreBuild = determinePreBuildAnimationIndex(structure);
    if (iDrawPreBuild < 0) return; // bail, don't draw invalid tiles

    int pixelWidth = structure->getWidthInPixels();
    int pixelHeight = structure->getHeightInPixels();

    // SDL_Surface *temp = SDL_CreateRGBSurface(0, pixelWidth, pixelHeight,32,0,0,0,255);
    // renderDrawer->FillWithColor(temp, SDL_Color{255,0,255,255});

    int drawX = structure->iDrawX();
    int drawY = structure->iDrawY();

    int scaledWidth = mapCamera->factorZoomLevel(pixelWidth);
    int scaledHeight = mapCamera->factorZoomLevel(pixelHeight);

    // Draw prebuild
    //draw_sprite(temp, (BITMAP *) gfxdata[iDrawPreBuild].dat, 0, 0);
    SDL_Rect src = {0,0,gfxdata->getTexture(iDrawPreBuild)->w, gfxdata->getTexture(iDrawPreBuild)->h};
    SDL_Rect dest= {drawX, drawY, scaledWidth, scaledHeight};
    renderDrawer->renderStrechSprite(gfxdata->getTexture(iDrawPreBuild), src, dest);
    //destroy_bitmap(temp);

    // Draw shadow of the prebuild animation
    // int shadowIndex = iDrawPreBuild + 1;
    // set_trans_blender(0,0,0,128);
    // BITMAP *shadow = (BITMAP *) gfxdata[shadowIndex].dat;

    // BITMAP *stretchedShadow = create_bitmap_ex(colorDepth, scaledWidth, scaledHeight);
    // clear_to_color(stretchedShadow, makecol(255, 0, 255));
    
    //renderDrawer->stretchSprite(gfxdata->getSurface(shadowIndex), nullptr, drawX, drawY, scaledWidth, scaledHeight,128);

    //allegroDrawer->drawTransSprite(stretchedShadow, bmp_screen, drawX, drawY);

    // destroy_bitmap(stretchedShadow);
}

void cStructureDrawer::drawStructureAnimation(cAbstractStructure *structure)
{
    if (!structure) return;
    structure->draw();
}

int cStructureDrawer::determinePreBuildAnimationIndex(cAbstractStructure *structure)
{
    assert(structure);
    int iBuildFase = structure->getBuildingFase();
    int height = structure->getHeight();
    int width = structure->getWidth();

    // prebuild
    if (iBuildFase == 1 ||
            iBuildFase == 3 ||
            iBuildFase == 5 ||
            iBuildFase == 7 ||
            iBuildFase == 9) {

        // determine what kind of prebuild picture should be used.
        if (width == 1 && height == 1) {
            return BUILD_PRE_1X1;
        }

        if (width == 2 && height == 2) {
            return BUILD_PRE_2X2;
        }

        if (width == 3 && height == 2) {
            return BUILD_PRE_3X2;
        }

        if (width == 3 && height == 3) {
            return BUILD_PRE_3X3;
        }
    }

    return -1;
}

void cStructureDrawer::drawStructureAnimationWindTrap(cAbstractStructure *structure)
{
    assert(structure);
    assert(structure->getType() == WINDTRAP);

    cWindTrap *windtrap = dynamic_cast<cWindTrap *>(structure);
    assert(windtrap);

    int pixelWidth = structure->getWidthInPixels();
    int pixelHeight = structure->getHeightInPixels();

    int drawX = structure->iDrawX();
    int drawY = structure->iDrawY();

    // structures are animated within the same source bitmap. The Y coordinates determine
    // what frame is being drawn. So multiply the height of the structure size times frame
    int iSourceY = pixelHeight * structure->getFrame();

    int fade = windtrap->getFade();
    // int screenDepth = bitmap_color_depth(bmp_screen);

    // BITMAP *wind=create_bitmap_ex(screenDepth, pixelWidth, pixelHeight);

    // clear_to_color(wind, makecol(255,0,255));

    // renderDrawer->blit(structure->getBitmap(), nullptr, 0, iSourceY, 0, 0, pixelWidth, pixelHeight);

    int scaledWidth = mapCamera->factorZoomLevel(pixelWidth);
    int scaledHeight = mapCamera->factorZoomLevel(pixelHeight);

    // BITMAP *shadow = structure->getShadowBitmap();
    // if (shadow) {
    //     set_trans_blender(0, 0, 0, 160);

    //     int colorDepth = bitmap_color_depth(bmp_screen);
    //     BITMAP *stretchedShadow = create_bitmap_ex(colorDepth, scaledWidth, scaledHeight);
    //     clear_to_color(stretchedShadow, makecol(255, 0, 255));

    //     allegroDrawer->maskedStretchBlit(shadow, stretchedShadow, 0, iSourceY, pixelWidth, pixelHeight,
    //                                      0, 0, scaledWidth, scaledHeight);

    //     allegroDrawer->drawTransSprite(stretchedShadow, bmp_screen, drawX, drawY);

    //     // destroy_bitmap(stretchedShadow);
    // }

    // renderDrawer->bitmap_replace_color(wind, SDL_Color{40, 40, 182,255}, SDL_Color{0, 0, (Uint8)fade,255});
    // renderDrawer->maskedStretchBlit(structure->getBitmap(), nullptr, 0,0,32,32 , drawX, drawY, scaledWidth, scaledHeight);
    SDL_Rect src = {0,0,32,32};
    SDL_Rect dest= {drawX, drawY, scaledWidth, scaledHeight};
    renderDrawer->renderStrechSprite(structure->getBitmap(), src, dest);    
 
//    renderDrawer->maskedStretchBlit(wind, bmp_screen, 0, 0, pixelWidth, pixelHeight, drawX, drawY, scaledWidth, scaledHeight);
    //renderDrawer->stretchSprite(structure->getShadowBitmap(), nullptr, drawX, drawY, scaledWidth, scaledHeight,128);

    // SDL_FreeSurface(wind);
}

void cStructureDrawer::drawStructureAnimationTurret(cAbstractStructure *structure)
{
    assert(structure);
    assert(structure->getType() == TURRET || structure->getType() == RTURRET);

    int iHeadFacing = -1;
    int facingAngles = 8;
    if (structure->getType() == TURRET) {
        cGunTurret *gunTurret = dynamic_cast<cGunTurret *>(structure);
        iHeadFacing = gunTurret->getHeadFacing();
        facingAngles = gunTurret->getFacingAngles();
    }
    else if (structure->getType() == RTURRET) {
        cRocketTurret *rocketTurret = dynamic_cast<cRocketTurret *>(structure);
        iHeadFacing = rocketTurret->getHeadFacing();
        facingAngles = rocketTurret->getFacingAngles();
    }
    assert(iHeadFacing > -1);

    // for now support these 2 facing angles amounts
    if (facingAngles == 16) {
        structure->setFrame(convertAngleToDrawIndex(iHeadFacing, false, 4, 16));
    }
    else if (facingAngles == 8) {
        structure->setFrame(convertAngleToDrawIndex(iHeadFacing, false, 2, 8));
    }

    // :-/
    if (game.isDebugMode()) {
        cPlayer &humanPlayer = players[HUMAN];
        cAbstractStructure *pStructure = humanPlayer.getSelectedStructure();
        if (pStructure && pStructure == structure) {
            cMouse *pMouse = game.getMouse();
            cGameControlsContext *pContext = humanPlayer.getGameControlsContext();

            int x1 = pMouse->getX();
            int y1 = pMouse->getY();
            int x2 = mapCamera->getWindowXPosition(structure->pos_x() + 16);
            int y2 = mapCamera->getWindowYPosition(structure->pos_y() + 16);

            renderDrawer->renderLine( x1, y1, x2, y2, SDL_Color{255, 255, 255,255});

            int mouseCellX = map.getCellX(pContext->getMouseCell());
            int mouseCellY = map.getCellY(pContext->getMouseCell());

            int cellX = map.getCellX(structure->getCell());
            int cellY = map.getCellY(structure->getCell());

            float degrees = fDegrees(cellX, cellY, mouseCellX, mouseCellY);

            int facingAngle = faceAngle(degrees, facingAngles);

            pMouse->addDebugLine(fmt::format("degrees = {}, faceAngle = {}", degrees, facingAngle));

//            int frame = convertAngleToDrawIndex(facingAngle, false, 4, 16);
            // override frame
//            structure->setFrame(frame);

            if (structure->getType() == TURRET) {
                cGunTurret *gunTurret = dynamic_cast<cGunTurret *>(structure);
                gunTurret->setShouldHeadFacing(facingAngle);
            }
            else if (structure->getType() == RTURRET) {
                cRocketTurret *rocketTurret = dynamic_cast<cRocketTurret *>(structure);
                rocketTurret->setShouldHeadFacing(facingAngle);
            }

        }
    }

    drawStructureAnimation(structure);
}

void cStructureDrawer::drawStructureAnimationRefinery(cAbstractStructure *structure)
{
    assert(structure);
    assert(structure->getType() == REFINERY);

    drawStructureAnimation(structure);
}

void cStructureDrawer::drawStructureForLayer(cAbstractStructure *structure, int layer)
{
    assert(structure);

    // always select proper palette (of owner)
    // select_palette(players[structure->getOwner()].pal);

    // when layer is <= 1 the building is just being placed. The prebuild
    // animation should be be drawn or, the normal drawing is shown (ie the
    // structure is not in action, like deploying harvester etc).

    // when stage == 2, it means only to draw the repair animation above the structure
    // this is done after all the structures have been drawn with stage 1 or lower. Causing
    // the repair icons to always overlap other structures. This is ugly, the repair icons
    // should be 'particles' (like smoke etc) instead of being hacked here!

    if (layer <= 1) {
        int iDrawPreBuild = determinePreBuildAnimationIndex(structure);

        // if no prebuild picture is selected, than we should draw the building
        // itself. The reason why the above if uses only buildfase 1, 3, 5, 7 and 9 means
        // that it will cause the switching between pre-build/building state as if the
        // building is being 'readied' after placement.
        if (iDrawPreBuild < 0) {
            // if (structure->getType() == WINDTRAP) {
            //     // draw windtrap
            //     drawStructureAnimationWindTrap(structure);
            // }
            // else 
            if (structure->getType() == TURRET || structure->getType() == RTURRET) {
                drawStructureAnimationTurret(structure);
            }
            else if (structure->getType() == REFINERY) {
                drawStructureAnimationRefinery(structure);
            }
            else {
                drawStructureAnimation(structure);
            }
            structure->drawFlags();
        }
        else {
            drawStructurePrebuildAnimation(structure);
        }
    }
    else if (layer == 2) {
        // TODO: REMOVE THIS CODE AND create particles for this
        // now draw the repair alpha when repairing
        if (structure->getType() == REPAIR && structure->hasUnitWithin()) {
            renderIconOfUnitBeingRepaired(structure);
        }

        if (structure->isRepairing()) {
            renderIconThatStructureIsBeingRepaired(structure);
        }
    }
}

void cStructureDrawer::renderIconThatStructureIsBeingRepaired(cAbstractStructure *structure) const
{
    int iconWidth = (gfxdata->getSurface(MOUSE_REPAIR))->w;
    int iconHeight = (gfxdata->getSurface(MOUSE_REPAIR))->h;
    int drawX = structure->iDrawX();
    int drawY = structure->iDrawY();
    int offsetX = (structure->getWidthInPixels() - iconWidth) / 2;
    int offsetY = (structure->getHeightInPixels() - iconHeight) / 2;
    int offsetXScaled = mapCamera->factorZoomLevel(offsetX);
    int offsetYScaled = mapCamera->factorZoomLevel(offsetY);
    int scaledWidth = mapCamera->factorZoomLevel(iconWidth);
    int scaledHeight = mapCamera->factorZoomLevel(iconHeight);
    //renderDrawer->stretchSprite(bmp_screen, gfxdata->getSurface(MOUSE_REPAIR), drawX+offsetXScaled, drawY + offsetYScaled, scaledWidth, scaledHeight);
    SDL_Rect src = {0,0,iconWidth,iconHeight};
    SDL_Rect dest = {drawX+offsetXScaled, drawY + offsetYScaled, scaledWidth, scaledHeight};
    renderDrawer->renderStrechSprite(gfxdata->getTexture(MOUSE_REPAIR), src, dest);
}

void cStructureDrawer::renderIconOfUnitBeingRepaired(cAbstractStructure *structure) const
{
    cRepairFacility *repairFacility = dynamic_cast<cRepairFacility *>(structure);
    assert(repairFacility);
    int unitId = repairFacility->getUnitIdWithin();
    cUnit &pUnit = unit[unitId];
    int iconId = pUnit.getUnitInfo().icon;

    int iconWidth = (gfxinter->getSurface(iconId))->w;
    int iconHeight = (gfxinter->getSurface(iconId))->h;
    // SDL_Surface *bmp = SDL_CreateRGBSurface(0,iconWidth, iconHeight,32,0,0,0,255);
    // renderDrawer->FillWithColor(bmp, SDL_Color{255,0,255,255});
    // renderDrawer->renderSprite(gfxinter->getTexture(iconId), 0, 0);

    // draw health bar of unit on top of icon?
    int draw_x = 3;
    int draw_y = iconHeight - 8;

    int width_x = iconWidth - 6;

    int height_y = 5;

    float healthNormalized = pUnit.getTempHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

    // bar itself
    renderDrawer->renderRectFillColor(draw_x, draw_y, width_x+1, height_y+1, 0,0,0,255);
    renderDrawer->renderRectFillColor(draw_x, draw_y, (w-1), height_y,r,g,32,255);

    // bar around it
    //_rect(bmp, draw_x, draw_y, draw_x + width_x, draw_y + height_y, SDL_Color{255, 255, 255));
    renderDrawer->renderRectColor(draw_x, draw_y, width_x, height_y, SDL_Color{255, 255, 255,255});
    int drawX = structure->iDrawX();
    int drawY = structure->iDrawY();
    int offsetX = (structure->getWidthInPixels() - iconWidth) / 2;
    int offsetY = (structure->getHeightInPixels() - iconHeight) / 2;
    int offsetXScaled = mapCamera->factorZoomLevel(offsetX);
    int offsetYScaled = mapCamera->factorZoomLevel(offsetY);
    int scaledWidth = mapCamera->factorZoomLevel(iconWidth);
    int scaledHeight = mapCamera->factorZoomLevel(iconHeight);
    SDL_Rect src = {0,0,iconWidth, iconHeight};
    SDL_Rect dest = {drawX + offsetXScaled, drawY + offsetYScaled, scaledWidth, scaledHeight};
    renderDrawer->renderStrechSprite(gfxinter->getTexture(iconId), src, dest);
    // SDL_FreeSurface(bmp);
}

void cStructureDrawer::drawStructuresForLayer(int layer)
{
    for (int i=0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *theStructure = structure[i];

        if (!theStructure) continue;

        if (structureUtils.isStructureVisibleOnScreen(theStructure)) {
            // draw
            drawStructureForLayer(theStructure, layer);

            cPlayer &player = players[HUMAN]; // TODO: Pass it as variable? (instead of getting it from here)
            // regardless if selected, render this so you know from which structure things will come?
            if (player.isPrimaryStructureForStructureType(theStructure->getType(), i)) {
                drawRectangleOfStructure(theStructure, player.getPrimaryBuildingFadingColor());
                continue;
            }

            if (i == player.selected_structure) {
                drawRectangleOfStructure(theStructure, player.getSelectFadingColor());
            }
        }
    }

    renderDrawer->renderRectFillColor((game.m_screenW - cSideBar::SidebarWidth), 0,
                                 cSideBar::SidebarWidth, game.m_screenH, 0, 0, 0,255);
}

void cStructureDrawer::drawStructureHealthBar(int iStructure)
{
    if (iStructure < 0 || iStructure >= MAX_STRUCTURES) return;

    cAbstractStructure *theStructure = structure[iStructure];

    if (!theStructure) {
        return;
    }

    // Draw structure health
    int draw_x = theStructure->iDrawX() - 1;
    int draw_y = theStructure->iDrawY() - 5;

    int widthBmp = mapCamera->factorZoomLevel(theStructure->getStructureInfo().bmp_width);
    int width_x = widthBmp - 1;

    int height_y = 4;

    float healthNormalized = theStructure->getHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

    // bar itself
    renderDrawer->renderRectFillColor(draw_x, draw_y, width_x+1, height_y+1, 0,0,0,255);
    renderDrawer->renderRectFillColor(draw_x, draw_y, (w-1), height_y, (Uint8)r,(Uint8)g,32,255);

    // bar around it
    //_rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, SDL_Color{255, 255, 255));
    renderDrawer->renderRectColor(draw_x, draw_y, width_x, height_y, SDL_Color{255, 255, 255,255});
}
