#include "../../include/d2tmh.h"

#include "utils/cSoundPlayer.h"

// Constructor
cStarPort::cStarPort() {
    // other variables (class specific)
    TIMER_deploy = 0;
    frigateDroppedPackage = false;
}

int cStarPort::getType() const {
    return STARPORT;
}

cStarPort::~cStarPort() {

}

void cStarPort::think() {
    // think like base class
    cAbstractStructure::think();

}

// think about units deployment animation
void cStarPort::think_deployment() {
    if (!isAnimating()) {
        return; // do nothing when not animating
    }

    if (frigateDroppedPackage) {
        iFrame = 7;
        return;
    }

    // Frigate is going towards starport so animate starport
    TIMER_flag++;

    if (TIMER_flag > 30) {
        TIMER_flag = 0;

        iFrame++;
        if (iFrame > 6) {
            iFrame = 0;
        }
    }
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cStarPort::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_deployment();
}

// FPS based thing function (every second called)
void cStarPort::think_deploy() {
    if (frigateDroppedPackage) {
        TIMER_deploy--;
        if (TIMER_deploy < 0) {
            TIMER_deploy = 1;
            // deploy unit
            cOrderProcesser *orderProcesser = players[iPlayer].getOrderProcesser();
            cBuildingListItem *item = orderProcesser->getItemToDeploy();
            if (item) {
                int cellToDeployTo = getNonOccupiedCellAroundStructure();
                orderProcesser->markOrderAsDeployed(item);
                item->decreaseTimesOrdered();
                int rallyPoint = getRallyPoint();

                if (cellToDeployTo >= 0) {
                    int id = UNIT_CREATE(cellToDeployTo, item->getBuildId(), iPlayer, true);
                    if (rallyPoint > -1) {
                        unit[id].move_to(rallyPoint, -1, -1);
                    }
                    const int house = players[iPlayer].getHouse();
                    game.playVoice(SOUND_VOICE_05_ATR, house); // unit deployed
                } else {
                    // could not find cell to deploy to, reinforce it
                    if (rallyPoint > -1) {
                        cellToDeployTo = rallyPoint;
                    }

                    if (cellToDeployTo < 0) {
                        // in this case, the structure has nothing around it free, but nor a rally point is set
                        // assume that the cell to drop is the location of the structure itself
                        cellToDeployTo = getCell();
                    }
                    int cellAtBorderOfMap = map.findCloseMapBorderCellRelativelyToDestinationCel(cellToDeployTo);
                    REINFORCE(iPlayer, item->getBuildId(), cellToDeployTo, cellAtBorderOfMap);
                }
            } else {
                // item is null, no more items to deploy.
                setAnimating(false);
                orderProcesser->setOrderHasBeenProcessed();
                iFrame = 0;
                frigateDroppedPackage = false;
            }
        }
    }
}

void cStarPort::think_guard() {
    return; // starport has no guard function
}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
