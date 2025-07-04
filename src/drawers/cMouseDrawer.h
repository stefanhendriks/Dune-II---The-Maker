#pragma once

#include "sMouseEvent.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "gui/MouseToolTip.h"
#include "gui/cTextWriter.h"

class cPlayer;
class cWindTrap;

class cMouseDrawer {
public:
    explicit cMouseDrawer(cPlayer *thePlayer);

    void draw();

    void drawToolTip();

    void onNotify(const s_MouseEvent &event);

protected:
    void drawToolTipBackground();
    void drawToolTipGeneralInformation(cAbstractStructure *theStructure, cTextWriter &textWriter);
    void drawToolTipWindTrapInformation(cWindTrap *theWindTrap, cTextWriter &textWriter);
    void drawToolTipSiloInformation(cAbstractStructure *theStructure, cTextWriter &textWriter);
    void drawToolTipTurretInformation(cAbstractStructure *theStructure, cTextWriter &textWriter);

    int getDrawXToolTip(int width);
    int getDrawYToolTip(int height);
    int getWidthToolTip();
    int getHeightToolTip();

private:
    void onMouseAt(const s_MouseEvent &event);

    cMouseToolTip m_mouseToolTip;
    cPlayer *player;
    int mouseX, mouseY;
};

