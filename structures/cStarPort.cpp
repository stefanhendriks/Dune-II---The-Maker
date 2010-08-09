#include "../d2tmh.h"

// Constructor
cStarPort::cStarPort()
{
 // other variables (class specific)
 TIMER_deploy = 0;
 frigateDroppedPackage = false;
}

int cStarPort::getType() {
	return STARPORT;
}

cStarPort::~cStarPort()
{

}

void cStarPort::think()
{
	// think like base class
	cAbstractStructure::think();

}

// think about units deployment animation
void cStarPort::think_deployment() {
	if (!isAnimating()) return; // do nothing when not animating
	if (!frigateDroppedPackage) {
		if (iFrame < 0) {
            iFrame = 1;
		}

        TIMER_flag++;

        if (TIMER_flag > 70) {
            TIMER_flag = 0;

            iFrame++;
			if (iFrame > 3) {
				iFrame = 1;
			}
        }
	} else {
        iFrame = 4;
	}
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cStarPort::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_deployment();
}

// called every second, to deploy a unit
// TODO: Reimplement deployment functionality
void cStarPort::think_deploy()
{
	if (frigateDroppedPackage) {
		  logbook("think deploy");
		  TIMER_deploy--;
		  if (TIMER_deploy < 0) {
			  TIMER_deploy = 1;
			  // deploy unit
			  cOrderProcesser * orderProcesser = player[iPlayer].getOrderProcesser();
			  cBuildingListItem * item = orderProcesser->getItemToDeploy();
			  if (item) {
				  cStructureUtils structureUtils;
				  int cellToDeployTo = iFreeAround();
				  orderProcesser->markOrderAsDeployed(item);
				  item->decreaseTimesOrdered();

				  if (cellToDeployTo >= 0) {
					  int id = UNIT_CREATE(cellToDeployTo, item->getBuildId(), iPlayer, true);
					  if (getRallyPoint() > -1) {
						  unit[id].move_to(getRallyPoint(), -1, -1);
					  }
					  play_voice(SOUND_VOICE_05_ATR); // unit deployed
				  } else {
					  // could not find cell to deploy to, reinforce it
					  cCellCalculator cellCalculator;
					  if (getRallyPoint() > -1) {
						  cellToDeployTo = getRallyPoint();
					  }
					  int cellAtBorderOfMap = cellCalculator.findCloseMapBorderCellRelativelyToDestinationCel(cellToDeployTo);
					  REINFORCE(0, item->getBuildId(), cellToDeployTo, cellAtBorderOfMap);
				  }
			  } else {
				  // item is null, no more items to deploy.
				  setAnimating(false);
				  orderProcesser->setOrderHasBeenProcessed();
				  frigateDroppedPackage = false;
			  }
		  }
	}
}

void cStarPort::think_guard() {
    return; // starport has no guard function
}

// Draw function to draw this structure()
void cStarPort::draw(int iStage) {
 cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
