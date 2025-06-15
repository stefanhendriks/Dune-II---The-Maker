#include "cMouseDrawer.h"

#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "gameobjects/structures/cRefinery.h"
#include "gameobjects/structures/cSpiceSilo.h"
#include "gameobjects/structures/cWindTrap.h"
#include "gui/cTextWriter.h"
#include "player/cPlayer.h"

#include <allegro.h>

#include <algorithm>

cMouseDrawer::cMouseDrawer(cPlayer * thePlayer) : m_mouseToolTip(thePlayer), player(thePlayer) {
    assert(thePlayer);
	mouseX = mouseY = 0;
}

void cMouseDrawer::draw() {
    game.getMouse()->draw();
}

int cMouseDrawer::getDrawXToolTip(int width) {
	int x = mouseX + 32;

	// correct drawing position so it does not fall off screen.
	int diffX = (x + width) - game.m_screenX;
	if (diffX > 0) {
		x-= diffX;
	}
	return x;
}

int cMouseDrawer::getDrawYToolTip(int height) {
	int y = mouseY + 32;

	// correct drawing position so it does not fall off screen.
	int diffY = (y + height) - game.m_screenY;
	if (diffY > 0) {
		y -= diffY;
	}
	return y;
}

int cMouseDrawer::getWidthToolTip() {
	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		return 150;
	}

	return 0;
}

int cMouseDrawer::getHeightToolTip() {
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

void cMouseDrawer::drawToolTip() {
	int height = getHeightToolTip();
	int width = getWidthToolTip();

	if (height <= 0|| width <= 0) {
		return;
	}

	int x = getDrawXToolTip(width);
	int y = getDrawYToolTip(height);

	cGameControlsContext * context = player->getGameControlsContext();

	if (context->isMouseOverStructure()) {
		auto textWriter = cTextWriter((x + 2), (y + 2), small_font, 12);

		cAbstractStructure * theStructure = context->getStructurePointerWhereMouseHovers();

		drawToolTipBackground();
		drawToolTipGeneralInformation(theStructure, textWriter);

//		// depending on structure type give more info
		int structureType = theStructure->getType();
		if (structureType == WINDTRAP) {
			cWindTrap * windTrap = dynamic_cast<cWindTrap*>(theStructure);
            assert(windTrap);
			drawToolTipWindTrapInformation(windTrap, textWriter);
		} else if (structureType == SILO || structureType == REFINERY) {
			drawToolTipSiloInformation(theStructure, textWriter);
		} else if (structureType == TURRET || structureType == RTURRET) {
			drawToolTipTurretInformation(theStructure, textWriter);
		}
	}
}

void cMouseDrawer::drawToolTipBackground() {
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
  red = std::max(red - 64, 0);
  green = std::max(green - 64, 0);
  blue = std::max(blue - 64, 0);

	color = makecol(red, green, blue);

  const auto black = allegroDrawer->getColor_BLACK();
  const auto white = makecol(255, 255, 255);
	rect(bmp_screen, x, y, x+(width-1), y + (height-1), white);
  allegroDrawer->drawRectangleTransparentFilled(bmp_screen, {x, y, width, height}, color, 128);
	int shadowX = x + width;
	int shadowY = y + height;
	allegroDrawer->drawRectangleTransparentFilled(bmp_screen, {x + 4, shadowY, (width - 4), 4}, black, 128);
	allegroDrawer->drawRectangleTransparentFilled(bmp_screen, {shadowX, y + 4, 4, height}, black, 128);
}

void cMouseDrawer::drawToolTipTurretInformation(cAbstractStructure * theStructure, cTextWriter& textWriter) {
	assert(theStructure);
	if (theStructure->belongsTo(player)) {
		textWriter.writeWithOneInteger("Sight : %d", theStructure->getSight());
		textWriter.writeWithOneInteger("Range : %d", theStructure->getRange());
	} else {
		textWriter.write("Sight : Unknown");
		textWriter.write("Range : Unknown");
	}
}

void cMouseDrawer::drawToolTipGeneralInformation(cAbstractStructure * theStructure, cTextWriter& textWriter) {
	assert(theStructure);
	const s_StructureInfo &structureType = theStructure->getStructureInfo();

	std::string description;
	if (theStructure->isPrimary()) {
		description=fmt::format("{} (PRIMARY)", structureType.name);
	} else {
		description=fmt::format("{}", structureType.name);
	}
	textWriter.write(description.c_str(), makecol(255, 255, 0));
	textWriter.writeWithTwoIntegers("Hitpoints : %d/%d", theStructure->getHitPoints(), theStructure->getMaxHP());
	textWriter.writeWithOneInteger("Armor : %d", theStructure->getArmor());
	textWriter.writeWithOneInteger("Protected : %d%%", (100-theStructure->getPercentageNotPaved()));
}

void cMouseDrawer::drawToolTipWindTrapInformation(cWindTrap * theWindTrap, cTextWriter& textWriter) {
	assert(theWindTrap);
	if (theWindTrap->getOwner() == HUMAN) {
		int powerOut = theWindTrap->getPlayer()->getPowerProduced();
		int powerUse = theWindTrap->getPlayer()->getPowerUsage();

		if (powerUse <= powerOut) {
			textWriter.writeWithTwoIntegers("Total usage : %d/%d (OK)", powerUse, powerOut);
		} else {
			textWriter.writeWithTwoIntegers("Total usage : %d/%d (LOW)", powerUse, powerOut);
		}
		textWriter.writeWithTwoIntegers("Windtrap outage : %d/%d", theWindTrap->getPowerOut(), theWindTrap->getMaxPowerOut());
	} else {
		textWriter.write("Total usage : Unknown");
		textWriter.write("Windtrap outage : Unknown");
	}
}

void cMouseDrawer::drawToolTipSiloInformation(cAbstractStructure * theStructure, cTextWriter& textWriter) {
	assert(theStructure);
	
	// TODO / IDEA --> Perhaps some 'spy on enemy intel' upgrade should be available for
	// house Ordos, so you can actually check on the enemy spice etc.
	if (theStructure->getOwner() == HUMAN) {
		int spiceCapacityOfStructure = 0;
		if (theStructure->getType() == REFINERY) {
			cRefinery * refinery = dynamic_cast<cRefinery*>(theStructure);
            assert(refinery);
			spiceCapacityOfStructure = refinery->getSpiceSiloCapacity();
		} else if (theStructure->getType() == SILO) {
			cSpiceSilo * spiceSilo = dynamic_cast<cSpiceSilo*>(theStructure);
            assert(spiceSilo);
			spiceCapacityOfStructure = spiceSilo->getSpiceSiloCapacity();
		}

		cPlayer *thePlayer = theStructure->getPlayer();
		int maxSpice = thePlayer->getMaxCredits();
		int currentSpice =  thePlayer->getCredits();
		if (currentSpice <= maxSpice) {
			textWriter.writeWithTwoIntegers("Total usage : %d/%d (OK)", currentSpice, maxSpice);
		} else {
			textWriter.writeWithTwoIntegers("Total usage : %d/%d (NOK)", currentSpice, maxSpice);
		}
		textWriter.writeWithTwoIntegers("Silo capacity : %d/%d", spiceCapacityOfStructure, 1000);
	} else {
		textWriter.write("Spice usage : Unknown");
	}
}

void cMouseDrawer::onMouseAt(const s_MouseEvent &event) {
    this->mouseX = event.coords.x;
    this->mouseY = event.coords.y;
}

void cMouseDrawer::onNotify(const s_MouseEvent &event) {
    if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
        onMouseAt(event);
    }
}
