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

void cStructureDrawer::drawRectangeOfStructure(cAbstractStructure * theStructure, int color) {
	assert(theStructure);
	int draw_x = theStructure->iDrawX();
	int draw_y = theStructure->iDrawY();
	int width_x = structures[theStructure->getType()].bmp_width-1;
	int height_y = structures[theStructure->getType()].bmp_height-1;
	rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, color);
}

void cStructureDrawer::drawStructuresForLayer(int layer) {
	// draw all structures
	cStructureUtils structureUtils;

	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {

			structureUtils.putStructureOnDimension(MAPID_STRUCTURES, theStructure);

			if (structureUtils.isStructureOnScreen(theStructure))
			{
				// draw
				theStructure->draw(layer);

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

	}

	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));
}
