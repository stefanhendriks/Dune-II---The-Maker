#include "cMouseDrawer.h"

#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/structures/cRefinery.h"
#include "gameobjects/structures/cSpiceSilo.h"
#include "gameobjects/structures/cWindTrap.h"
#include "gui/cTextWriter.h"
#include "player/cPlayer.h"

#include <SDL2/SDL.h>

#include <algorithm>

cMouseDrawer::cMouseDrawer(cPlayer *thePlayer) : m_mouseToolTip(thePlayer), player(thePlayer)
{
    assert(thePlayer);
    mouseX = mouseY = 0;
}

void cMouseDrawer::draw()
{
    game.getMouse()->draw();
}

int cMouseDrawer::getDrawXToolTip(int width)
{
    int x = mouseX + 32;

    // correct drawing position so it does not fall off screen.
    int diffX = (x + width) - game.m_screenW;
    if (diffX > 0) {
        x-= diffX;
    }
    return x;
}

int cMouseDrawer::getDrawYToolTip(int height)
{
    int y = mouseY + 32;

    // correct drawing position so it does not fall off screen.
    int diffY = (y + height) - game.m_screenH;
    if (diffY > 0) {
        y -= diffY;
    }
    return y;
}

int cMouseDrawer::getWidthToolTip()
{
    cGameControlsContext *context = player->getGameControlsContext();

    if (context->isMouseOverStructure()) {
        return 150;
    }

    return 0;
}

int cMouseDrawer::getHeightToolTip()
{
    cGameControlsContext *context = player->getGameControlsContext();

    if (context->isMouseOverStructure()) {
        cAbstractStructure *theStructure = context->getStructurePointerWhereMouseHovers();

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

void cMouseDrawer::drawToolTip()
{
    int height = getHeightToolTip();
    int width = getWidthToolTip();

    if (height <= 0|| width <= 0) {
        return;
    }

    int x = getDrawXToolTip(width);
    int y = getDrawYToolTip(height);

    cGameControlsContext *context = player->getGameControlsContext();

    if (context->isMouseOverStructure()) {
        auto textWriter = cTextWriter((x + 2), (y + 2), small_font, 12);

        cAbstractStructure *theStructure = context->getStructurePointerWhereMouseHovers();

        drawToolTipBackground();
        drawToolTipGeneralInformation(theStructure, textWriter);

//		// depending on structure type give more info
        int structureType = theStructure->getType();
        if (structureType == WINDTRAP) {
            cWindTrap *windTrap = dynamic_cast<cWindTrap *>(theStructure);
            assert(windTrap);
            drawToolTipWindTrapInformation(windTrap, textWriter);
        }
        else if (structureType == SILO || structureType == REFINERY) {
            drawToolTipSiloInformation(theStructure, textWriter);
        }
        else if (structureType == TURRET || structureType == RTURRET) {
            drawToolTipTurretInformation(theStructure, textWriter);
        }
    }
}

void cMouseDrawer::drawToolTipBackground()
{
    cGameControlsContext *context = player->getGameControlsContext();

    int width = getWidthToolTip();
    int height = getHeightToolTip();

    int x = getDrawXToolTip(width);
    int y = getDrawYToolTip(height);

    SDL_Color color = player->getMinimapColor();

    if (context->isMouseOverStructure()) {
        cAbstractStructure *theStructure = context->getStructurePointerWhereMouseHovers();

        color = theStructure->getPlayer()->getMinimapColor();

        // TODO: Think about this, it does not really look nice to me, rather see house color
//		// make grey when not our own
//		if (theStructure->getPlayer()->getId() != player->getId()) {
//			red = 64;
//			green = 64;
//			blue = 64;
//		}
    }

    unsigned char red = color.r;
    unsigned char green = color.g;
    unsigned char blue = color.b;

    // tone down a bit
    red = std::max(red - 64, 0);
    green = std::max(green - 64, 0);
    blue = std::max(blue - 64, 0);

    color = SDL_Color{red, green, blue,255};
	// @Mira what do this code ?
    const auto black = renderDrawer->getColor_BLACK();
    const auto white = SDL_Color{255, 255, 255,255};
    //_rect(bmp_screen, x, y, x+(width-1), y + (height-1), white);
    //renderDrawer->drawRect(bmp_screen, x, y, width-1, height-1, white);
    renderDrawer->drawRectTransparentFilled(bmp_screen, {x, y, width, height}, color, 128);
    int shadowX = x + width;
    int shadowY = y + height;
    renderDrawer->drawRectTransparentFilled(bmp_screen, {x + 4, shadowY, (width - 4), 4}, black, 128);
    renderDrawer->drawRectTransparentFilled(bmp_screen, {shadowX, y + 4, 4, height}, black, 128);
}

void cMouseDrawer::drawToolTipTurretInformation(cAbstractStructure *theStructure, cTextWriter &textWriter)
{
    assert(theStructure);
    if (theStructure->belongsTo(player)) {
        textWriter.write(fmt::format("Sight : {}", theStructure->getSight()));
        textWriter.write(fmt::format("Range : {}", theStructure->getRange()));
    }
    else {
        textWriter.write("Sight : Unknown");
        textWriter.write("Range : Unknown");
    }
}

void cMouseDrawer::drawToolTipGeneralInformation(cAbstractStructure *theStructure, cTextWriter &textWriter)
{
    assert(theStructure);
    const s_StructureInfo &structureType = theStructure->getStructureInfo();

    std::string description;
    if (theStructure->isPrimary()) {
        description=fmt::format("{} (PRIMARY)", structureType.name);
    }
    else {
        description=fmt::format("{}", structureType.name);
    }
    textWriter.write(description.c_str(), SDL_Color{255, 255, 0,255});
    textWriter.writef("Hitpoints : {}/{}", theStructure->getHitPoints(), theStructure->getMaxHP());
    textWriter.writef("Armor : {}", theStructure->getArmor());
    textWriter.writef("Protected : {}%%", (100-theStructure->getPercentageNotPaved()));
}

void cMouseDrawer::drawToolTipWindTrapInformation(cWindTrap *theWindTrap, cTextWriter &textWriter)
{
    assert(theWindTrap);
    if (theWindTrap->getOwner() == HUMAN) {
        int powerOut = theWindTrap->getPlayer()->getPowerProduced();
        int powerUse = theWindTrap->getPlayer()->getPowerUsage();

        if (powerUse <= powerOut) {
            textWriter.writef("Total usage : {}/{} (OK)", powerUse, powerOut);
        }
        else {
            textWriter.writef("Total usage : {}/{} (LOW)", powerUse, powerOut);
        }
        textWriter.writef("Windtrap outage : {}/{}", theWindTrap->getPowerOut(), theWindTrap->getMaxPowerOut());
    }
    else {
        textWriter.write("Total usage : Unknown");
        textWriter.write("Windtrap outage : Unknown");
    }
}

void cMouseDrawer::drawToolTipSiloInformation(cAbstractStructure *theStructure, cTextWriter &textWriter)
{
    assert(theStructure);

    // TODO / IDEA --> Perhaps some 'spy on enemy intel' upgrade should be available for
    // house Ordos, so you can actually check on the enemy spice etc.
    if (theStructure->getOwner() == HUMAN) {
        int spiceCapacityOfStructure = 0;
        if (theStructure->getType() == REFINERY) {
            cRefinery *refinery = dynamic_cast<cRefinery *>(theStructure);
            assert(refinery);
            spiceCapacityOfStructure = refinery->getSpiceSiloCapacity();
        }
        else if (theStructure->getType() == SILO) {
            cSpiceSilo *spiceSilo = dynamic_cast<cSpiceSilo *>(theStructure);
            assert(spiceSilo);
            spiceCapacityOfStructure = spiceSilo->getSpiceSiloCapacity();
        }

        cPlayer *thePlayer = theStructure->getPlayer();
        int maxSpice = thePlayer->getMaxCredits();
        int currentSpice =  thePlayer->getCredits();
        if (currentSpice <= maxSpice) {
            textWriter.writef("Total usage : {}/{} (OK)", currentSpice, maxSpice);
        }
        else {
            textWriter.writef("Total usage : {}/{} (NOK)", currentSpice, maxSpice);
        }
        textWriter.writef("Silo capacity : {}/{}", spiceCapacityOfStructure, 1000);
    }
    else {
        textWriter.write("Spice usage : Unknown");
    }
}

void cMouseDrawer::onMouseAt(const s_MouseEvent &event)
{
    this->mouseX = event.coords.x;
    this->mouseY = event.coords.y;
}

void cMouseDrawer::onNotify(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
        onMouseAt(event);
    }
}
