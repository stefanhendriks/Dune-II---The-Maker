#pragma once

#include "sMouseEvent.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "gui/cTextWriter.h"

class cPlayer;
class cWindTrap;

class cMouseDrawer {
public:
    explicit cMouseDrawer(cPlayer *thePlayer, cTextDrawer *textDrawer);

    void draw();

    void drawToolTip();

    void onNotify(const s_MouseEvent &event);

protected:
    void drawToolTipBackground();
    void drawToolTipGeneralInformation(cAbstractStructure *theStructure);
    void drawToolTipWindTrapInformation(cWindTrap *theWindTrap);
    void drawToolTipSiloInformation(cAbstractStructure *theStructure);
    void drawToolTipTurretInformation(cAbstractStructure *theStructure);

    int getDrawXToolTip(int width);
    int getDrawYToolTip(int height);
    int getWidthToolTip();
    int getHeightToolTip();

private:
    void onMouseAt(const s_MouseEvent &event);

    cPlayer *player;
    int mouseX, mouseY;
    cTextWriter* textWriter = nullptr;
};

