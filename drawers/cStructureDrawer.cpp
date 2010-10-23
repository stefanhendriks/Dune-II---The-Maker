/*
 * cStructureDrawer.cpp
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

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

void cStructureDrawer::drawStructuresForLayer(int layer) {
	// draw all structures
	cStructureUtils structureUtils;

	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {

			structureUtils.putStructureOnDimension(MAPID_STRUCTURES, theStructure);

			int structureDrawX = theStructure->iDrawX();
			int structureDrawY = theStructure->iDrawY();
			int widthInPixels = structureUtils.getStructureWidthInPixels(theStructure);

			if (((theStructure->iDrawX()+structures[theStructure->getType()].bmp_width) > 0 && theStructure->iDrawX() < game.screen_x) &&
				((theStructure->iDrawY()+structures[theStructure->getType()].bmp_height) > 0 && theStructure->iDrawY() < game.screen_y))
			{
				// draw
				theStructure->draw(layer);

				if (player[0].iPrimaryBuilding[theStructure->getType()] == i)
				{
					alfont_textprintf(bmp_screen, game_font, theStructure->iDrawX()-1, theStructure->iDrawY()-1, makecol(0,0,0), "P");
					alfont_textprintf(bmp_screen, game_font, theStructure->iDrawX(), theStructure->iDrawY(), makecol(255,255,255), "P");
				}


				// When mouse hovers over this structure
				if (((mouse_x >= theStructure->iDrawX() && mouse_x <= (theStructure->iDrawX() + structures[theStructure->getType()].bmp_width))) &&
					((mouse_y >= theStructure->iDrawY() && mouse_y <= (theStructure->iDrawY() + structures[theStructure->getType()].bmp_height))))
				{
					// draw some cool id of it
					if (key[KEY_D] && key[KEY_TAB])
					{
						alfont_textprintf(bmp_screen, game_font, theStructure->iDrawX(),theStructure->iDrawY(), makecol(255,255,255), "%d", i);
						alfont_textprintf(bmp_screen, game_font, theStructure->iDrawX(),theStructure->iDrawY()-16, makecol(255,255,255), "%s", structures[theStructure->getType()].name);
					}


					game.hover_structure=i;
				}

				if (i == game.selected_structure)
				{
					// draw rectangle around it fading, like dune 2
					int draw_x = theStructure->iDrawX();
					int draw_y = theStructure->iDrawY();
					int width_x = structures[theStructure->getType()].bmp_width-1;
					int height_y = structures[theStructure->getType()].bmp_height-1;

					rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, makecol(game.fade_select, game.fade_select, game.fade_select));
				}


			}

		}

	}

	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));
}
