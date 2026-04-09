#include "cUnitUtils.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "map/cMap.h"
#include "definitions.h"
#include "gameobjects/units/cUnits.h"

cUnitUtils::cUnitUtils()
{
}

cUnitUtils &cUnitUtils::getInstance()
{
    static cUnitUtils unitUtils;
    return unitUtils;
}

// find the first unit of type belonging to player Id.
int cUnitUtils::findUnit(int type, int iPlayerId)
{
    return findUnit(type, iPlayerId, -1);
}

// find the first unit of type belonging to player Id, and is not the same Id as iIgnoreUnitId.
int cUnitUtils::findUnit(int type, int iPlayerId, int iIgnoreUnitId)
{
    for (int i=0; i < game.m_gameObjectsContext->getUnits().size(); i++) {
        if (i == iIgnoreUnitId || !game.m_gameObjectsContext->getUnits()[i].isValid()) {
            continue;
        }

        if (game.m_gameObjectsContext->getUnits()[i].iPlayer == iPlayerId && game.m_gameObjectsContext->getUnits()[i].iType == type) {
            return i;
        }
    }
    return -1;
}
