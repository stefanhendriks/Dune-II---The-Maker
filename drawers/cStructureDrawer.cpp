/*
 * cStructureDrawer.cpp
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cStructureDrawer::cStructureDrawer() {
}

cStructureDrawer::~cStructureDrawer() {
}

void cStructureDrawer::drawStructuresFirstLayer() {
	drawStructuresForLayer(0);
}

void cStructureDrawer::drawStructuresSecondLayer() {
	drawStructuresForLayer(2);
}

void cStructureDrawer::drawStructuresHealthBars() {
	cGameControlsContext * context = player[HUMAN].getGameControlsContext();

    // DRAW HEALTH
    if (context->isMouseOverStructure())
    {
        int i = context->getIdOfStructureWhereMouseHovers();
        drawStructureHealthBar(i);
    }
}

void cStructureDrawer::drawRectangeOfStructure(cAbstractStructure * theStructure, int color) {
	assert(theStructure);
	int drawX = theStructure->iDrawX();
	int drawY = theStructure->iDrawY();
    int width = structures[theStructure->getType()].bmp_width - 1;
    int height = structures[theStructure->getType()].bmp_height - 1;

    int width_x = mapCamera->factorZoomLevel(width);
    int height_y = mapCamera->factorZoomLevel(height);

	rect(bmp_screen, drawX, drawY, drawX + width_x, drawY + height_y, color);
}

void cStructureDrawer::drawStructurePrebuildAnimation(cAbstractStructure * structure) {
	int iDrawPreBuild = determinePreBuildAnimationIndex(structure);

	int drawX = structure->iDrawX();
	int drawY = structure->iDrawY();

    // Draw prebuild
    draw_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild].dat, drawX, drawY);

    // Draw shadow of the prebuild animation
    if (iDrawPreBuild != BUILD_PRE_CONST) {
        set_trans_blender(0,0,0,128);
        draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[iDrawPreBuild + 1].dat, drawX, drawY);
    }

}

void cStructureDrawer::drawStructureAnimation(cAbstractStructure * structure) {
	if (!structure) return;

    int orgWidthInPixels = structure->getWidthInPixels();
    int orgHeightInPixels = structure->getHeightInPixels();

    // structures are animated within the same source bitmap. The Y coordinates determine
    // what frame is being drawn. So multiply the height of the structure size times frame
    int iSourceY = orgHeightInPixels * structure->getFrame();

    int drawX = structure->iDrawX();
    int drawY = structure->iDrawY();

    int widthInPixels = mapCamera->factorZoomLevel(orgWidthInPixels);
    int heightInPixels = mapCamera->factorZoomLevel(orgHeightInPixels);

    if (structure->getShadowBitmap()) {
        set_trans_blender(0, 0, 0, 128);
        allegroDrawer->maskedStretchBlit(structure->getShadowBitmap(), bmp_screen, 0, iSourceY, orgWidthInPixels, orgHeightInPixels, drawX, drawY, widthInPixels, heightInPixels);
    }

    allegroDrawer->maskedStretchBlit(structure->getBitmap(), bmp_screen, 0, iSourceY, orgWidthInPixels, orgHeightInPixels, drawX, drawY, widthInPixels, heightInPixels);
}

int cStructureDrawer::determinePreBuildAnimationIndex(cAbstractStructure * structure) {
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

void cStructureDrawer::drawStructureAnimationWindTrap(cAbstractStructure * structure) {
	assert(structure);
	assert(structure->getType() == WINDTRAP);

	cWindTrap * windtrap = dynamic_cast<cWindTrap*>(structure);

	assert(structure);

	int orgWidthInPixels = structure->getWidthInPixels();
	int orgHeightInPixels = structure->getHeightInPixels();

	int drawX = structure->iDrawX();
	int drawY = structure->iDrawY();

    // structures are animated within the same source bitmap. The Y coordinates determine
    // what frame is being drawn. So multiply the height of the structure size times frame
    int iSourceY = orgHeightInPixels * structure->getFrame();

	int fade = windtrap->getFade();
    int screenDepth = bitmap_color_depth(bmp_screen);

	BITMAP *temp=create_bitmap_ex(8, orgWidthInPixels, orgHeightInPixels);
	BITMAP *temp_shadow=create_bitmap(orgWidthInPixels, orgHeightInPixels);
    BITMAP *wind=create_bitmap(orgWidthInPixels, orgHeightInPixels);
    BITMAP *temp_result=create_bitmap_ex(screenDepth, orgWidthInPixels, orgHeightInPixels);

	clear(temp);
    clear_to_color(wind, makecol(255,0,255));
	clear_to_color(temp_shadow, makecol(255,0,255));

    // the 'background' of the temp_result is from bmp_screen
    // BUG BUG: This will break when zooming, this only works when unzoomed (it copies the expected terrain)
    blit(bmp_screen, temp_result, drawX, drawY, 0, 0, orgWidthInPixels, orgHeightInPixels);

    blit(structure->getBitmap(), temp, 0, iSourceY, 0, 0, orgWidthInPixels, orgHeightInPixels);

	// in case shadow, prepare shadow bitmap in memory
	if (structure->getShadowBitmap()) {
		blit(structure->getShadowBitmap(), temp_shadow, 0, iSourceY, 0, 0, orgWidthInPixels, orgHeightInPixels);
	}

    int widthInPixels = mapCamera->factorZoomLevel(orgWidthInPixels);
    int heightInPixels = mapCamera->factorZoomLevel(orgHeightInPixels);

	// windtrap animation
	draw_sprite(wind, temp, 0, 0);
	lit_windtrap_color(wind, makecol(0, 0, fade));
    draw_sprite(temp_result, temp, 0, 0);
    allegroDrawer->stretchBlit(temp_result, bmp_screen, 0, 0, orgWidthInPixels, orgHeightInPixels, drawX, drawY, widthInPixels, heightInPixels);

	// in case shadow, draw shadow now using fBlend.
	if (structure->getShadowBitmap()) {
		fblend_trans(temp_shadow, bmp_screen, drawX, drawY, 128);
	}


	// destroy used bitmaps
	destroy_bitmap(temp);
	destroy_bitmap(temp_shadow);
	destroy_bitmap(temp_result);
    destroy_bitmap(wind);
}

void cStructureDrawer::drawStructureAnimationTurret(cAbstractStructure * structure) {
	assert(structure);
	assert(structure->getType() == TURRET || structure->getType() == RTURRET);

	int iHeadFacing = -1;
	if (structure->getType() == TURRET) {
		cGunTurret * gunTurret = dynamic_cast<cGunTurret *>(structure);
		iHeadFacing = gunTurret->getHeadFacing();
	} else if (structure->getType() == RTURRET) {
		cRocketTurret * rocketTurret = dynamic_cast<cRocketTurret *>(structure);
		iHeadFacing = rocketTurret->getHeadFacing();
	}
	assert(iHeadFacing > -1);

	structure->setFrame(convert_angle(iHeadFacing));

	drawStructureAnimation(structure);
}

void cStructureDrawer::drawStructureAnimationRefinery(cAbstractStructure * structure) {
	assert(structure);
	assert(structure->getType() == REFINERY);

	int oldFrame = structure->getFrame();

	// Refinery has content, then the frame is a bit different
	if (structure->hasUnitWithin()) {
		structure->setFrame(structure->getFrame() + 5);
	}

	drawStructureAnimation(structure);

	// restore iFrame again
	structure->setFrame(oldFrame);
}

void cStructureDrawer::drawStructureForLayer(cAbstractStructure * structure, int layer) {
	assert(structure);

	// always select proper palette (of owner)
    select_palette(player[structure->getOwner()].pal);

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
        	if (structure->getType() == WINDTRAP) {
        		// draw windtrap
        		drawStructureAnimationWindTrap(structure);
        	} else if (structure->getType() == TURRET || structure->getType() == RTURRET) {
        		drawStructureAnimationTurret(structure);
        	} else if (structure->getType() == REFINERY) {
        		drawStructureAnimationRefinery(structure);
        	} else {
        		drawStructureAnimation(structure);
        	}
        } else {
        	drawStructurePrebuildAnimation(structure);
        }
	}
	else if (layer == 2) {
		// TODO: REMOVE THIS CODE AND create particles for this
		// now draw the repair alpha when repairing
		if (structure->isRepairing()) {
			if (structure->getRepairAlpha() > -1) {
                int iconWidth = ((BITMAP *)gfxdata[MOUSE_REPAIR].dat)->w;
                int iconHeight = ((BITMAP *)gfxdata[MOUSE_REPAIR].dat)->h;
                int drawX = structure->iDrawX();
				int drawY = structure->iDrawY();
				int repairX = mapCamera->factorZoomLevel(structure->getRepairX());
				int repairY = mapCamera->factorZoomLevel(structure->getRepairY());
                int scaledWidth = mapCamera->factorZoomLevel(iconWidth);
                int scaledHeight = mapCamera->factorZoomLevel(iconHeight);
                stretch_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_REPAIR].dat, drawX+repairX, drawY + repairY, scaledWidth, scaledHeight);
			} else {
				structure->setRepairAlpha(rnd(255));
			}
		}
	}
}

void cStructureDrawer::drawStructuresForLayer(int layer) {
	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (!theStructure) continue;

        structureUtils.putStructureOnDimension(MAPID_STRUCTURES, theStructure);

        if (structureUtils.isStructureVisibleOnScreen(theStructure)) {
            // draw
            drawStructureForLayer(theStructure, layer);
            //theStructure->draw(layer);

            if (player[0].iPrimaryBuilding[theStructure->getType()] == i) {
                int color = theStructure->getPlayer()->getMinimapColor();
                // TODO: change it into fading color or something other than this ugly rectangle.
                drawRectangeOfStructure(theStructure, color);
            }

            if (i == game.selected_structure) {
                drawRectangeOfStructure(theStructure, makecol(game.fade_select, game.fade_select, game.fade_select));
            }
        }
	}

	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));
}

void cStructureDrawer::drawStructureHealthBar(int iStructure) {
    if (iStructure < 0 || iStructure >= MAX_STRUCTURES) return;

	cAbstractStructure * theStructure = structure[iStructure];

	if (!theStructure) {
	    return;
	}

	// Draw structure health
	int draw_x = theStructure->iDrawX() - 1;
	int draw_y = theStructure->iDrawY() - 5;

	int widthBmp = mapCamera->factorZoomLevel(theStructure->getS_StructuresType().bmp_width);
	int width_x = widthBmp - 1;

	int height_y = 4;

    float healthNormalized = theStructure->getHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

	// shadow
   // rectfill(bmp_screen, draw_x+2, draw_y+2, draw_x + width_x + 2, draw_y + height_y + 2, makecol(0,0,0));

	// bar itself
	rectfill(bmp_screen, draw_x, draw_y, draw_x + width_x+1, draw_y + height_y+1, makecol(0,0,0));
	rectfill(bmp_screen, draw_x, draw_y, draw_x + (w-1), draw_y + height_y, makecol(r,g,32));

	// bar around it
	rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, makecol(255, 255, 255));
}
