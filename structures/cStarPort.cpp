#include "../d2tmh.h"

// Constructor
cStarPort::cStarPort()
{

 // other variables (class specific)
 TIMER_deploy = 1;
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

	if (TIMER_deploy < 0) {
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
//    logbook("think deploy");
//    if (TIMER_deploy > -1)
//	{
//		TIMER_deploy--;
//        logbook("timer deploy was > -1");
//
//		if (TIMER_deploy == 0)
//		{
//            logbook("timer deploy == 0");
//			bool bDeployed=false;
//
//            // deploy stuff
//            for (int i=0; i < MAX_ICONS; i++)
//            {
//                if (game.iconFrigate[i] > 0)
//                {
//                    int iID = game.iconlist[LIST_STARPORT][i].iUnitID;
//
//                    // find free space
//                    int iNewCell = iFreeAround();
//
//					if (iNewCell > -1)
//					{
//						int id = UNIT_CREATE(iNewCell, iID, 0, true);
//
//						if (getRallyPoint() > -1) {
//                            unit[id].move_to(getRallyPoint(), -1, -1);
//						}
//
//						bDeployed=true;
//						game.iconFrigate[i]--;
//						play_voice(SOUND_VOICE_05_ATR); // unit deployed
//						break;
//					}
//					else
//					{
//
//						// when nothing found now, it means the structure is the only
//						// one. So, we cannot dump it. Send over a reinforcement
//						int rX = (iCellGiveX(getCell()) - 5) + rnd(10);
//						int rY = (iCellGiveY(getCell()) - 5) + rnd(10);
//
//                        if (getRallyPoint() > -1)
//                        {
//                         //   unit[id].move_to(iRallyPoint, -1, -1);
//                          rX = (iCellGiveX(getRallyPoint()) - 2) + rnd(4);
//                          rY = (iCellGiveY(getRallyPoint()) - 2) + rnd(4);
//						}
//
//						FIX_BORDER_POS(rX, rY);
//
//						REINFORCE(0, iID, iCellMake(rX, rY), iCellMake(rX, rY));
//
//						bDeployed=true;
//						game.iconFrigate[i]--;
//						break;
//					}
//			}
//		}
//
//
//
//            if (bDeployed)
//            {
//                logbook("Deployed a unit?");
//                TIMER_deploy=2;
//                game.TIMER_mayorder = 2;
//            }
//            else
//            {
//                // its empty, so we are done
//                iFrame=0;
//                setAnimating(false);
//                TIMER_deploy=-1;
//                game.TIMER_mayorder=-1;
//            }
//		} // deploy a unit
//	}
}

void cStarPort::think_guard() {
    return; // starport has no guard function
}

// Draw function to draw this structure()
void cStarPort::draw(int iStage) {
 cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
