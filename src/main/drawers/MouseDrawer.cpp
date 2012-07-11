#include "../include/d2tmh.h"

MouseDrawer::MouseDrawer(cPlayer *thePlayer, Mouse *theMouse) {
	assert(thePlayer);
	assert(theMouse);
	player = thePlayer;
	context = thePlayer->getGameControlsContext();
	assert(context);
	mouse = theMouse;
	mouseToolTip = new MouseToolTip(player, mouse);
}

MouseDrawer::~MouseDrawer() {
	player = NULL;
	mouse = NULL;
	delete mouseToolTip;
	mouseToolTip = NULL;
}

void MouseDrawer::draw() {
	drawMouseTile();
	drawMouseRectangle();
}

void MouseDrawer::drawMouseRectangle() {
	if (context->isMouseOnBattleField()) {
		if (mouse->isMouseDraggingRectangle()) {
			Rectangle * rectangle = mouse->getCurrentDrawingRectangle();
			rect(bmp_screen, rectangle->getStartX(), rectangle->getStartY(), rectangle->getEndX(), rectangle->getEndY(), makecol(255, 255, 255));
		}
	}
}

void MouseDrawer::drawMouseTile() {
	int x = mouse->getX();
	int y = mouse->getY();
	int mouse_tile = mouse->getMouseTile();

	// adjust coordinates of drawing according to the specific mouse sprite/tile
	if (mouse_tile == MOUSE_DOWN) {
		y -= 16;
	} else if (mouse_tile == MOUSE_RIGHT) {
		x -= 16;
	} else if (mouse_tile == MOUSE_MOVE || mouse_tile == MOUSE_RALLY) {
		x -= 16;
		y -= 16;
	} else if (mouse_tile == MOUSE_ATTACK) {
		x -= 16;
		y -= 16;
	} else if (mouse_tile == MOUSE_REPAIR) {
		x -= 16;
		y -= 16;
	} else if (mouse_tile == MOUSE_PICK) {
		x -= 16;
		y -= 16;
	}

	draw_sprite(bmp_screen, (BITMAP *) gfxdata[mouse_tile].dat, x, y);
}

int MouseDrawer::getDrawXToolTip(int width) {
	int x = mouse->getX() + 32;

	// correct drawing position so it does not fall off screen.
	int w = /* game.getScreenResolution()->getWidth()  800 ;*/ 100;
	int diffX = (x + width) - w;
	if (diffX > 0) {
		x -= diffX;
	}
	return x;
}

int MouseDrawer::getDrawYToolTip(int height) {
	int y = mouse->getY() + 32;

	// correct drawing position so it does not fall off screen.
	int h = /* game.getScreenResolution()->getHeight 600 */ 600;
	int diffY = (y + height) - h;
	if (diffY > 0) {
		y -= diffY;
	}
	return y;
}

int MouseDrawer::getWidthToolTip() {
	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		return 150;
	}

	return 0;
}

int MouseDrawer::getHeightToolTip() {
	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		cAbstractStructure * theStructure = context->getStructurePointerWhereMouseHovers();

		int height = 50;
		switch (theStructure->getType()) {
			case WINDTRAP:
				height = 78;
				break;
			case REFINERY:
				height = 78;
				break;
			case SILO:
				height = 78;
				break;
			case TURRET:
				height = 78;
				break;
			case RTURRET:
				height = 78;
				break;
			default:
				height = 50;
				break;
		}

		return height;
	}

	return 0;
}

void MouseDrawer::drawToolTip() {
	int height = getHeightToolTip();
	int width = getWidthToolTip();

	if (height <= 0 || width <= 0) {
		return;
	}

	int x = getDrawXToolTip(width);
	int y = getDrawYToolTip(height);

	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		TextWriter * textWriter = new TextWriter((x + 2), (y + 2), small_font, 12);

		cAbstractStructure * theStructure = context->getStructurePointerWhereMouseHovers();

		drawToolTipBackground();
		drawToolTipGeneralInformation(theStructure, textWriter);

		// depending on structure type give more info
		int structureType = theStructure->getType();
		if (structureType == WINDTRAP) {
			cWindTrap * windTrap = dynamic_cast<cWindTrap*> (theStructure);
			drawToolTipWindTrapInformation(windTrap, textWriter);
		} else if (structureType == SILO || structureType == REFINERY) {
			drawToolTipSiloInformation(theStructure, textWriter);
		} else if (structureType == TURRET || structureType == RTURRET) {
			drawToolTipTurretInformation(theStructure, textWriter);
		}

		delete textWriter;
	}
}

void MouseDrawer::drawToolTipBackground() {
	cGameControlsContext * context = player->getGameControlsContext();

	int width = getWidthToolTip();
	int height = getHeightToolTip();

	int x = getDrawXToolTip(width);
	int y = getDrawYToolTip(height);

	int color = player->getMinimapColor();

	if (context->isMouseOverStructure()) {
		cAbstractStructure * theStructure = context->getStructurePointerWhereMouseHovers();

		color = theStructure->getPlayer()->getMinimapColor();

		// TODO: Think about this, it does not really look nice to me, rather see house color
		//		// make grey when not our own
		//		if (theStructure->getPlayer()->getId() != player->getId()) {
		//			red = 64;
		//			green = 64;
		//			blue = 64;
		//		}
	}

	int red = getr(color);
	int green = getg(color);
	int blue = getb(color);

	// tone down a bit
	cSimpleCalculator simpleCalculator;
	red = simpleCalculator.substractWithFloor(red, 64, 0);
	green = simpleCalculator.substractWithFloor(green, 64, 0);
	blue = simpleCalculator.substractWithFloor(blue, 64, 0);

	color = makecol(red, green, blue);

	//	fblend_rect_trans(bmp_screen, x, y, width, height, makecol(0,0,0), 128);
	rect(bmp_screen, x, y, x + (width - 1), y + (height - 1), makecol(255, 255, 255));
	fblend_rect_trans(bmp_screen, x, y, width, height, color, 128);
	int shadowX = x + width;
	int shadowY = y + height;
	fblend_rect_trans(bmp_screen, x + 4, shadowY, (width - 4), 4, makecol(0, 0, 0), 128);
	fblend_rect_trans(bmp_screen, shadowX, y + 4, 4, height, makecol(0, 0, 0), 128);
}

void MouseDrawer::drawToolTipTurretInformation(cAbstractStructure * theStructure, TextWriter *textWriter) {
	assert(theStructure);
	assert(textWriter);
	if (theStructure->getPlayer()->getId() == player->getId()) {
		textWriter->writeWithOneInteger("Sight : %d", theStructure->getSight());
		textWriter->writeWithOneInteger("Range : %d", theStructure->getRange());
	} else {
		textWriter->write("Sight : Unknown");
		textWriter->write("Range : Unknown");
	}
}

void MouseDrawer::drawToolTipGeneralInformation(cAbstractStructure * theStructure, TextWriter *textWriter) {
	assert(theStructure);
	assert(textWriter);
	s_Structures structureType = theStructure->getS_StructuresType();

	char description[255];
	memset(description, 0, sizeof(description));
	if (theStructure->isPrimary()) {
		sprintf(description, "%s (PRIMARY)", structureType.name);
	} else {
		sprintf(description, "%s", structureType.name);
	}
	textWriter->write(description, makecol(255, 255, 0));
	textWriter->writeWithTwoIntegers("Hitpoints : %d/%d", theStructure->getHitPoints(), theStructure->getMaxHP());
	textWriter->writeWithOneInteger("Armor : %d", theStructure->getArmor());
	textWriter->writeWithOneInteger("Protected : %d%%", (100 - theStructure->getPercentageNotPaved()));
}

void MouseDrawer::drawToolTipWindTrapInformation(cWindTrap * theWindTrap, TextWriter *textWriter) {
	assert(theWindTrap);
	assert(textWriter);
	if (theWindTrap->getOwner() == HUMAN) {
		int powerOut = theWindTrap->getPlayer()->has_power;
		int powerUse = theWindTrap->getPlayer()->use_power;

		if (powerUse <= powerOut) {
			textWriter->writeWithTwoIntegers("Total usage : %d/%d (OK)", powerUse, powerOut);
		} else {
			textWriter->writeWithTwoIntegers("Total usage : %d/%d (LOW)", powerUse, powerOut);
		}
		textWriter->writeWithTwoIntegers("Windtrap outage : %d/%d", theWindTrap->getPowerOut(), theWindTrap->getMaxPowerOut());
	} else {
		textWriter->write("Total usage : Unknown");
		textWriter->write("Windtrap outage : Unknown");
	}
}

void MouseDrawer::drawToolTipSiloInformation(cAbstractStructure * theStructure, TextWriter *textWriter) {
	assert(theStructure);
	assert(textWriter);

	// TODO / IDEA --> Perhaps some 'spy on enemy intel' upgrade should be available for
	// house Ordos, so you can actually check on the enemy spice etc.
	if (theStructure->getOwner() == HUMAN) {
		int spiceCapacityOfStructure = 0;
		if (theStructure->getType() == REFINERY) {
			cRefinery * refinery = dynamic_cast<cRefinery*> (theStructure);
			spiceCapacityOfStructure = refinery->getSpiceSiloCapacity();
		} else if (theStructure->getType() == SILO) {
			cSpiceSilo * spiceSilo = dynamic_cast<cSpiceSilo*> (theStructure);
			spiceCapacityOfStructure = spiceSilo->getSpiceSiloCapacity();
		}

		cPlayer *thePlayer = theStructure->getPlayer();
		int maxSpice = thePlayer->max_credits;
		int currentSpice = thePlayer->credits;
		if (currentSpice <= maxSpice) {
			textWriter->writeWithTwoIntegers("Total usage : %d/%d (OK)", currentSpice, maxSpice);
		} else {
			textWriter->writeWithTwoIntegers("Total usage : %d/%d (NOK)", currentSpice, maxSpice);
		}
		textWriter->writeWithTwoIntegers("Silo capacity : %d/%d", spiceCapacityOfStructure, 1000);
	} else {
		textWriter->write("Spice usage : Unknown");
	}
}
