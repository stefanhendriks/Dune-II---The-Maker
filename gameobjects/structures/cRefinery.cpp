#include "../../include/d2tmh.h"

// Constructor
cRefinery::cRefinery()
{


 // other variables (class specific)

}

int cRefinery::getType() {
	return REFINERY;
}

cRefinery::~cRefinery()
{

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cRefinery::think()
{

     // When a refinery, and expecting a unit
     if (iUnitID > -1)
        {
			int iMyID=-1;
			for (int i=0; i < MAX_STRUCTURES; i++)
				if (structure[i] == this)
				{
					iMyID=i;
					break;
				}

            // when the unit somehow does not go to us anymore, stop animating
            if (unit[iUnitID].isValid() == false)
            {
                iUnitID=-1;
				setAnimating(false);
                return;
            }

			if (unit[iUnitID].iStructureID != iMyID)
			{
				iUnitID=-1;
				setAnimating(false);
				return;
			}

            // the unit id is filled in, that means the unit is IN this structure
            // the TIMER_harvest of the unit will be used to dump the harvest in the
            // refinery

            unit[iUnitID].TIMER_harvest++;

            cPlayerDifficultySettings * difficultySettings = player[getOwner()].getDifficultySettings();

            if (unit[iUnitID].TIMER_harvest > difficultySettings->getDumpSpeed(10))
            {
                unit[iUnitID].TIMER_harvest = 0;

                // dump credits
                if (unit[iUnitID].iCredits > 0)
                {
                    int iAmount = 5;

                    // cap at max
                    if (unit[iUnitID].iCredits > units[unit[iUnitID].iType].credit_capacity)
                        unit[iUnitID].iCredits = units[unit[iUnitID].iType].credit_capacity;


                    if ((unit[iUnitID].iCredits - iAmount) < 0)
                    {
                        iAmount = unit[iUnitID].iCredits;
                    }

					if (player[unit[iUnitID].iPlayer].credits < player[unit[iUnitID].iPlayer].max_credits)
                    {
						player[unit[iUnitID].iPlayer].credits += iAmount;

                        // TODO: update harvested amopunt
                    }

                    unit[iUnitID].iCredits -= iAmount;
                }
                else
                {
					int iNewCell = iFreeAround();

					if (iNewCell > -1)
					{
						unit[iUnitID].iCell = iNewCell;

                        // let player know...
						if (unit[iUnitID].iPlayer==0)
							play_voice(SOUND_VOICE_02_ATR);

						// place back on map now
						//map.cell[iCell].id[MAPID_UNITS] = iUnitID;
					}
					else
					{
						logbook("Could not find space for this unit");

						// TODO: make carryall pick this up
						return;
					}

                    // done & restore unit
                    unit[iUnitID].iCredits = 0;
                    unit[iUnitID].iStructureID = -1;
                    unit[iUnitID].iHitPoints = unit[iUnitID].iTempHitPoints;
                    unit[iUnitID].iTempHitPoints = -1;
                    unit[iUnitID].iGoalCell = unit[iUnitID].iCell;
					unit[iUnitID].iPathIndex = -1;

					unit[iUnitID].TIMER_movewait = 0;
					unit[iUnitID].TIMER_thinkwait = 0;

                    if (DEBUGGING)
                        assert(iUnitID > -1);

                    map.cell[unit[iUnitID].iCell].id[MAPID_UNITS] = iUnitID;

					// perhaps we can find a carryall to help us out
					int iHarvestCell = UNIT_find_harvest_spot(iUnitID);

					if (iHarvestCell)
					{
						int iCarry = CARRYALL_TRANSFER(iUnitID, iHarvestCell);

						if (iCarry < 0)
						{
							unit[iUnitID].iGoalCell = iHarvestCell;
						}
						else
						{
							unit[iUnitID].TIMER_movewait = 100;
							unit[iUnitID].TIMER_thinkwait = 100;
						}
					}

					iUnitID=-1;
                }


            }


        }

	// think like base class
	cAbstractStructure::think();

}

void cRefinery::think_harvester_deploy() {
	if (!isAnimating()) return; // do nothing when not animating
    // harvester stuff
	if (iFrame < 0)  {
        iFrame = 1;
	}

    TIMER_flag++;

    if (TIMER_flag > 70) {
        TIMER_flag = 0;

        iFrame++;
		if (iFrame > 4) {
			iFrame = 1;
		}
	}
}

void cRefinery::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_harvester_deploy();
}

void cRefinery::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cRefinery::getSpiceSiloCapacity() {
	cHitpointCalculator hitpointCalculator;
	float percentage = ((float)getHitPoints() / (float)structures[getType()].hp);
	return hitpointCalculator.getByPercent(1000, percentage);
}
