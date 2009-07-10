/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"


// TODO: constructor/destructors

// cAIPlayer functions
void cAIPlayer::init(int iID)
{
    ID = iID;

	// SKIRMISH RELATED SETTINGS
	if (iID > 1)
		bPlaying=false;
	else
		bPlaying=true;

	iUnits=1;

    iCheckingPlaceStructure=-1;

	// -- END

    // iBuildingUnit[TRIKE] > 0 = its building a trike (progress!)    
    memset(iBuildingUnit, -1, sizeof(iBuildingUnit));
	memset(iBuildingStructure, -1, sizeof(iBuildingStructure));
    memset(TIMER_BuildUnit, -1, sizeof(TIMER_BuildUnit));    
	memset(TIMER_BuildStructure, -1, sizeof(TIMER_BuildStructure));	

    TIMER_attack=-(900 + rnd(200));
   //  TIMER_attack=-50;	

    TIMER_BuildUnits=-500; // give player advantage to build his stuff first, before computer grows his army
    TIMER_harv=200;
    TIMER_repair=500;
}

void cAIPlayer::BUILD_STRUCTURE(int iStrucType)
{
	for (int i=0; i < MAX_STRUCTURETYPES; i++)
		if (iBuildingStructure[i] > -1)
		{
			// already building
			return;
		}

	// not building

	// check if its allowed at all
	if (player[ID].iStructures[CONSTYARD] < 1)
		return; 

	if (player[ID].credits < structures[iStrucType].cost)
		return; // cannot buy

	iBuildingStructure[iStrucType]=0;
	TIMER_BuildStructure[iStrucType]=0; // start building

	player[ID].credits-= structures[iStrucType].cost;

	if (DEBUGGING)
	{
		logbook("Building STRUCTURE: ");
		logbook(structures[iStrucType].name);
	}
	
}

void cAIPlayer::BUILD_UNIT(int iUnitType)
{
    // Fix up house mixtures
    if (player[ID].house == HARKONNEN || player[ID].house == SARDAUKAR)
    {
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
        if (iUnitType == TRIKE) iUnitType = QUAD;
    }

	// In mission 9 there are only WOR's, so make a special hack hack for that ;)
	if (game.iMission >= 8)
	{
        if (iUnitType == INFANTRY) iUnitType = TROOPERS;
        if (iUnitType == SOLDIER) iUnitType = TROOPER;
	}

    if (player[ID].house == ORDOS)
    {
        if (iUnitType == TRIKE) iUnitType = RAIDER;        
    }

       

    bool bAllowed = AI_UNITSTRUCTURETYPE(ID, iUnitType);

    if (bAllowed == false)
        return; // do not go further

    // when building a tank, etc, check if we do not already build
    bool bAlreadyBuilding=false;
    for (int i=0; i < MAX_UNITTYPES; i++)
    {        
        // when building a quad
        if (iUnitType == QUAD || iUnitType == TRIKE || iUnitType == RAIDER)
        {
            // the same
            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                    bAlreadyBuilding=true;
        }

        // when building a tank or something
        if (iUnitType == TANK || iUnitType == LAUNCHER || iUnitType == SIEGETANK ||
            iUnitType == SONICTANK || iUnitType == DEVASTATOR || iUnitType == HARVESTER)
        {
            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                    bAlreadyBuilding=true;
        }

        // when building a carryall
        if (iUnitType == CARRYALL || iUnitType == ORNITHOPTER)
        {
            if (i == iUnitType)
                if (iBuildingUnit[i] > -1)
                bAlreadyBuilding=true;

        }
    }

    //if (bAlreadyBuilding)
      //  return;

    // Now build it
    iBuildingUnit[iUnitType]=0;                  // start building!
    player[ID].credits -= units[iUnitType].cost; // pay for it
	
	if (DEBUGGING)
	{
		logbook("Building UNIT: ");
		logbook(units[iUnitType].name);
	}
}


void cAIPlayer::think_building()
{

	if (ID == 0)
		return; // human player does not think

    /* 
		structure building; 

		when building completed, search for a spot and place it!
	*/
    for (int i=0; i < MAX_STRUCTURETYPES; i++)
	{
		if (iBuildingStructure[i] > -1)
		{
			int iTimerCap=35; // was 35


            if (DEBUGGING)
                iTimerCap=7;

			// the more constyards
			iTimerCap /= (1+(player[ID].iStructures[CONSTYARD]/2));

			TIMER_BuildStructure[i]++;

			if (TIMER_BuildStructure[i] > iTimerCap)
			{
				iBuildingStructure[i]++;
				TIMER_BuildStructure[i]=0;
			}

			if (iBuildingStructure[i] >= structures[i].build_time)
			{
				// find place to place structure
				if (game.bSkirmish)
				{
					int iCll=iPlaceStructureCell(i);

					if (iCll > -1)
					{
						iBuildingStructure[i]=-1;
						TIMER_BuildStructure[i]=-1;
						cStructureFactory::getInstance()->createStructure(iCll, i, ID, 100);
					}
					else
					{
						// cannot place structure now, this sucks big time. return money
						iBuildingStructure[i]=-1;
						TIMER_BuildStructure[i]=0;
						player[ID].credits+= structures[i].cost;
					}
				}
			}
			// now break the loop, as we may only do one building at a time!
			break;
		}
	}



    
	/* 
		unit building
	*/
    for (int i=0; i < MAX_UNITTYPES; i++)
    {
        if (iBuildingUnit[i] > -1)
        {
            int iStrucType = AI_STRUCTYPE(i);

            TIMER_BuildUnit[i]++;

            int iTimerCap=35;

            iTimerCap /= (1+(player[ID].iStructures[iStrucType]/2));

			iTimerCap = player[ID].iBuildSpeed(iTimerCap);

            if (TIMER_BuildUnit[i] > iTimerCap)
            {
                iBuildingUnit[i]++;
                TIMER_BuildUnit[i]=0; // set to 0 again
            }

            if (iBuildingUnit[i] >= units[i].build_time)
            {
            //logbook("DONE BUILDING");

            // produce now
            int iStr = player[ID].iPrimaryBuilding[iStrucType];

            // no primary building yet, assign one
            if (iStr < 0)			
                iStr = FIND_PRIMARY_BUILDING(iStrucType, ID);

            if (iStr > -1)
            {
                int iProducedUnit=-1;

                if (structure[iStr])
                {
                if (structure[iStr]->iFreeAround() -1)
                {
                    int iSpot = structure[iStr]->iFreeAround();
                    player[ID].iPrimaryBuilding[iStrucType] = iStr;
                    structure[iStr]->bAnimate=true; // animate
                    iProducedUnit=UNIT_CREATE(iSpot, i, ID, false);                
                }
                else
                {
                    int iNewStr = FIND_PRIMARY_BUILDING(iStrucType, ID);

                    // assign new primary
                    if (iNewStr != iStr && iNewStr > -1)
                    {
                        int iSpot = structure[iNewStr]->iFreeAround();                        
                        player[ID].iPrimaryBuilding[iStrucType] = iNewStr;
                        structure[iNewStr]->bAnimate=true; // animate
                        iProducedUnit=UNIT_CREATE(iSpot, i, ID, false);                
                    }
					else
					{
						// nothing found, deliver the unit as is.

					}
                }
                }
                else
                {
                    player[ID].iPrimaryBuilding[iStrucType]=-1;
                }
                
                // produce                
                iBuildingUnit[i] = -1;

                // Assign to team (for AI attack purposes)
                unit[iProducedUnit].iGroup=rnd(3)+1;
            }
			else			
			{
				// deliver unit by carryall			
				for (int s=0; s < MAX_STRUCTURES; s++)
				{
					if (structure[s])
						if (structure[s]->iPlayer == ID)
							if (structure[s]->getType() == iStrucType)
							{
								REINFORCE(ID, i, structure[s]->getCell(), -1);
							}
				}

				// produce                
                iBuildingUnit[i] = -1;
			}

        }

        }        
    }

	// END OF THINK BUILDING
}

void cAIPlayer::think_harvester()
{
 
 if (TIMER_harv > 0)
 {
    TIMER_harv--;
    return;
 }

 TIMER_harv=200;

 // think about spice blooms 
 int iBlooms=-1;

 for (int c=0; c < MAX_CELLS; c++)
	 if (map.cell[c].type == TERRAIN_BLOOM)
		 iBlooms++;

 // When no blooms are detected, we must 'spawn' one
 if (iBlooms < 3)
 {
	int iCll = rnd(MAX_CELLS);
	
	if (map.cell[iCll].type == TERRAIN_SAND)
	{
		// create bloom
		map.create_spot(iCll, TERRAIN_BLOOM, 0);
	}

 }
 else
 {	 
	if (rnd(100) < 15)
	{

	// if we got a unit to spare (something light), we can blow it up  by walking over it
	// with a soldier or something
	int iUnit=-1;
	int iDist=9999;
	int iBloom =  CLOSE_SPICE_BLOOM(-1);
	for (int i=0; i < MAX_UNITS; i++)
	{
		if (unit[i].isValid()){
			if (unit[i].iPlayer > 0 &&
				unit[i].iPlayer == ID && unit[i].iAction == ACTION_GUARD) {

				if (units[unit[i].iType].infantry) {
					int d = ABS_length(	iCellGiveX(iBloom), 
										iCellGiveY(iBloom), 
										iCellGiveX(unit[i].iCell), 
										iCellGiveY(unit[i].iCell));
					
					if (d < iDist)
					{
						iUnit = i;
						iDist = d;
					}
				}
			}
		}

	}

	if (iUnit > -1)
		{			
            // shoot spice bloom
		    UNIT_ORDER_ATTACK(iUnit, iBloom,-1, -1, iBloom);            
		}
		else
			BUILD_UNIT(SOLDIER);

	}	 
 }

 
 bool bFoundHarvester=false;
 
 if (player[ID].iStructures[REFINERY] > 0)
 {
     bFoundHarvester=false;
     
         for (int j = 0; j < MAX_UNITS; j++)
         {
             if (unit[j].isValid())
                 if (unit[j].iPlayer == ID)
                 {
                     if (unit[j].iType == HARVESTER)
                     {
                         bFoundHarvester=true;                         
                         break;
                     }

                     else if (unit[j].iType == CARRYALL)
                     {
                        if (unit[j].iUnitID > -1)
                            if (unit[unit[j].iUnitID].iType == HARVESTER)
                            {
                                bFoundHarvester=true;
                                break;
                            }

                        if (unit[j].iNewUnitType == HARVESTER)
                        {
                            bFoundHarvester=true;
                            break;
                        }
                     }

                 }


         }

         
 
        if (bFoundHarvester == false)
        {   
            for (int k=0; k < MAX_STRUCTURES; k++)
            {
                if (structure[k])
                    if (structure[k]->iPlayer == ID)
                        if (structure[k]->getType() == REFINERY)
                        {                            
                            REINFORCE(ID, HARVESTER, structure[k]->getCell(), -1);
                            
                        }

            }
        }

     } // has refinery

 
 
}

void cAIPlayer::think()
{
    think_building();

    // not time yet to think
    if (player[ID].TIMER_think < 10) {
        player[ID].TIMER_think++;
        return;
    }

    player[ID].TIMER_think = 0;

    // think about fair harvester stuff
    think_harvester();

    if (ID == 0)
        return; // we do not think further
    
    // depening on player, do thinking
    if (ID == AI_WORM) {
        if (rnd(100) < 25)
            think_worm();

        return;
    }

    // Now think about building stuff etc
	think_buildbase();
	think_buildarmy();
    think_attack();
}

void cAIPlayer::think_repair()
{
    if (TIMER_repair > 0)
    {
        TIMER_repair--;
        return;
    }
    
    TIMER_repair=500; // next timed interval to think about this...

    // check if we must repair, only if we have a repair structure ofcourse
    // and we have some money to spare for repairs
    if (player[ID].iStructures[REPAIR] > 0 && player[ID].credits > 250)
    {
        // yes, we can repair
        for (int i=0; i < MAX_UNITS; i++)
            if (unit[i].isValid())
                if (unit[i].iPlayer == ID)
                    if (unit[i].iHitPoints < units[unit[i].iType].hp)
                    {
                        // head off to repair
                         int iNewID = STRUCTURE_FREE_TYPE(ID, unit[i].iCell, REPAIR);

                         if (iNewID > -1)
                         {   
                             int iCarry = CARRYALL_TRANSFER(i, structure[iNewID]->getCell()+2);
                             
                             
                             if (iCarry > -1)                                 
                             {
                                 // yehaw we will be picked up!
                                 unit[i].TIMER_movewait = 100;
                                 unit[i].TIMER_thinkwait = 100;	                                
                             }
                             else
                             {
                                 logbook("Order move #3");
                                 UNIT_ORDER_MOVE(i, structure[iNewID]->getCell());   
                             }

                             unit[i].iStructureID = iNewID;
                             unit[i].iGoalCell = structure[iNewID]->getCell();                             
                         }

                    }
                        
    }

    // check if any structures where breaking down due decay    
}

void cAIPlayer::think_attack()
{
 if (TIMER_attack < 0)
 {
    TIMER_attack++;
    think_repair();
    return;
 }

 TIMER_attack = -(rnd(600)+100);
// TIMER_attack=-50;

 // find anyone with a specific random group number and send it to attack the player (focus cell)
 int iAmount=3 + rnd(8);
 int iTarget=-1;
 bool bUnit=false;
 int iGroup=rnd(3)+1;
 bool bInfantryOnly=false;

 int iAttackPlayer=0;

 if (game.bSkirmish)
 {
	 // skirmish games will make the ai a bit more agressive!
	 iAmount+=3 + rnd(7);
	 
	// check what players are playing
	 int iPl[5];
	 int iPlID=0;
	 memset(iPl, -1, sizeof(iPl));

	 for (int p=0; p < AI_WORM; p++)
	 {		 
		if (p != ID)
		{
			bool bOk=false;
			if (p == 0) bOk=true;
			if (p > 1 && aiplayer[p].bPlaying) bOk=true;

			if (player[p].iTeam != player[ID].iTeam)
			{
				iPl[iPlID] = p;
				iPlID++;
			}
		}
	 }

	 	iAttackPlayer=iPl[rnd(iPlID)];
		if (DEBUGGING)
		{
			char msg[255];
			sprintf(msg, "Attacking player id %d", iAttackPlayer);
			logbook(msg);
		}
 }

 // only when ai has a wor/barracks to regenerate his troops, send off infantry sometimes.
 if (player[ID].iStructures[WOR] > 0 ||
     player[ID].iStructures[BARRACKS])
     if (rnd(100) < 30)
     {
     bInfantryOnly=true;
     iAmount=10;
     }

 if (rnd(100) < 50)
 {
     iTarget = AI_RANDOM_STRUCTURE_TARGET(ID, iAttackPlayer);
     
     if (iTarget < 0)
     {
         iTarget = AI_RANDOM_UNIT_TARGET(ID, iAttackPlayer);
         bUnit=true;
     }
 }
 else
 {
     iTarget = AI_RANDOM_UNIT_TARGET(ID,iAttackPlayer);
     bUnit=true;
     
     if (iTarget < 0)
     {
         iTarget = AI_RANDOM_STRUCTURE_TARGET(ID, iAttackPlayer);
         bUnit=false;
     }     
 }

 if (iTarget < 0)
 {
	 if (DEBUGGING)
		 logbook("No target!");

	 
    return;
 }

 // make up the balance
 int iWe=0;
 int iThem=0;

 for (int i=0; i < MAX_UNITS; i++)
 {
	if (unit[i].isValid())
		if (unit[i].iPlayer == 0)
		{
			if (unit[i].iType != HARVESTER &&
				unit[i].iType != CARRYALL)
				iThem++;
		}
		else
		{
			if (unit[i].iType != HARVESTER &&
				unit[i].iType != CARRYALL)
				iWe++;
		}
 }

 // When WE do not even match THEM, we wait until we have build more
 if (iWe <= iThem && rnd(100) < 50)
 {
	 if (DEBUGGING)
		 logbook("AI: Decission to wait, no use to attack opponent which is equal or stronger then AI");

     TIMER_attack = -250;

	 return;
 } 

 int iUnits=0;

 for (int i=0; i < MAX_UNITS; i++)
 {
   if (unit[i].isValid())
       if (unit[i].iPlayer == ID && 
		   unit[i].iType != HARVESTER && unit[i].iType != CARRYALL)
       {		   

           if (bInfantryOnly == false)
           {

            if (unit[i].iGroup == iGroup)
            {
                if (bUnit)
                    UNIT_ORDER_ATTACK(i, unit[iTarget].iCell, iTarget, -1, -1);
                else
                    UNIT_ORDER_ATTACK(i, structure[iTarget]->getCell(), -1, iTarget, -1);

                iUnits++;
            }
           }
           else
           {
               if (units[unit[i].iType].infantry)
               {
                   if (bUnit)
                       UNIT_ORDER_ATTACK(i, unit[iTarget].iCell, iTarget, -1, -1);
                   else
                       UNIT_ORDER_ATTACK(i, structure[iTarget]->getCell(), -1, iTarget, -1);
               }
           }
       }
 }

 // get remaining units (not always)
 if (iUnits < iAmount &&
	 rnd(100) < 10 && player[ID].credits > 700)
 {
     for (int i=0; i < MAX_UNITS; i++)
     {
         if (unit[i].isValid())
             if (unit[i].iPlayer == ID)
             {
                 if (unit[i].iType != HARVESTER &&
                     unit[i].iType != CARRYALL && 
                     unit[i].iType != FRIGATE &&
                     unit[i].iAction == ACTION_GUARD)
                 {
                     if (bUnit)
                         UNIT_ORDER_ATTACK(i, unit[iTarget].iCell, iTarget, -1, -1);
                     else
                         UNIT_ORDER_ATTACK(i, structure[iTarget]->getCell(), -1, iTarget, -1);
                                     
                     iUnits++;
                     if (iUnits >= iAmount)
                         break;
                 }
             }
     }

 }
 
}

void cAIPlayer::think_buildarmy()
{
    // prevent human player thinking
    if (ID == 0)
        return; // do not build for human! :)
    

    if (TIMER_BuildUnits < 3)
    {
        TIMER_BuildUnits++;
        return;
    }

    /*
	if (game.bSkirmish)
	{
		if (player[ID].credits < 300)
			return;
	}*/

    TIMER_BuildUnits=0;

    // Depending on the mission/tech level, try to build stuff
    int iMission = game.iMission;
	int iChance = 10;

	if (player[ID].house == HARKONNEN || 
        player[ID].house == SARDAUKAR)
	{
		if (iMission <= 2)
			iChance=50;
		else
			iChance=10;
	}
	else
	{	
        // non trooper house(s)
		if (iMission <= 2)
			iChance=20;
	}
    
    // Skirmish override
    if (game.bSkirmish)
        iChance=10;

    if (iMission > 1 && rnd(100) < iChance)
    {
        if (player[ID].credits > units[INFANTRY].cost)
        {            
            BUILD_UNIT(INFANTRY); // (INFANTRY->TROOPERS CONVERSION IN FUNCTION)
        }
        else 
            BUILD_UNIT(SOLDIER); // (SOLDIER->TROOPER CONVERSION IN FUNCTION)
    }


	iChance=50;
	
	// low chance on buying the higher the mission
	if (iMission > 4) iChance = 30;
	if (iMission > 6) iChance = 20;
	if (iMission > 7) iChance = 10;
    if (iMission > 8) iChance = 5;

    // Skirmish override
    if (game.bSkirmish)
        iChance=7;

	// build quads / trikes
    if (iMission > 2 && rnd(100) < iChance)
    {
        if (player[ID].credits > units[QUAD].cost)
        {  
			BUILD_UNIT(QUAD); 
        }
        else if (player[ID].credits > units[TRIKE].cost)
		{		
			BUILD_UNIT(TRIKE); 
		}
    }

    int iHarvs = 0;     // harvesters
    int iCarrys= 0;     // carryalls
    
    if (iMission > 3)
    {
        // how many harvesters do we own?
        for (int i=0; i < MAX_UNITS; i++)
            if (unit[i].isValid())
                if (unit[i].iPlayer == ID && unit[i].iType == HARVESTER)
                    iHarvs++;

        if (iHarvs < player[ID].iStructures[REFINERY])
        {
            if (player[ID].credits > units[HARVESTER].cost)
                BUILD_UNIT(HARVESTER); // build harvester
        }
        else if (iHarvs >= player[ID].iStructures[REFINERY])
        {
            // enough harvesters , try to get ratio 2 harvs - 1 refinery
            if (iHarvs < (player[ID].iStructures[REFINERY] * 2))
                if (rnd(100) < 30)
                    BUILD_UNIT(HARVESTER);
        }
            
    }

    // ability to build carryalls
    if (iMission >= 5)
    {
        if (player[ID].credits > units[CARRYALL].cost)
        {

        for (int i=0; i < MAX_UNITS; i++)
            if (unit[i].isValid())
                if (unit[i].iPlayer == ID && unit[i].iType == CARRYALL)
                    iCarrys++;

        int iLimit = 1;
        
        if (iHarvs > 1 )
            iLimit = iHarvs / 2;

        if (iCarrys < iLimit)
        {
            // randomly, build
            if (rnd(100) < 50) // it is pretty wise to do so, so high chance of doing so...
                BUILD_UNIT(CARRYALL);
        }


        }

    }

    if (iMission > 6)
        if (player[ID].house == ATREIDES)
            if (player[ID].credits > units[ORNITHOPTER].cost)
                if (rnd(100) < 15)
                {
                    BUILD_UNIT(ORNITHOPTER);
                }

	if (iMission >= 8 || game.bSkirmish)
    {
        int iSpecial = DEVASTATOR;

        if (player[ID].house == ATREIDES)  iSpecial = SONICTANK;
        if (player[ID].house == ORDOS)     iSpecial = DEVIATOR;

        if (player[ID].credits > units[iSpecial].cost)
        {
            BUILD_UNIT(iSpecial);
        }        
        if (player[ID].credits > units[SIEGETANK].cost)
        {
            // enough to buy launcher , tank
            int nr = rnd(100);
            if (nr < 30)
                BUILD_UNIT(LAUNCHER);
            else if (nr > 30 && nr < 60)
                BUILD_UNIT(TANK);
            else if (nr > 60)
                BUILD_UNIT(SIEGETANK);
        }
        else if (player[ID].credits > units[LAUNCHER].cost)
        {
            if (rnd(100) < 50)
                BUILD_UNIT(LAUNCHER);
            else
                BUILD_UNIT(TANK);
        }
        else if (player[ID].credits > units[TANK].cost)
        {
            BUILD_UNIT(TANK);
        }
    }


    if (iMission == 4 && rnd(100) < 70)
    {
        if (player[ID].credits > units[TANK].cost)
            BUILD_UNIT(TANK);         
    }

    if (iMission == 5)
    {
        if (player[ID].credits > units[LAUNCHER].cost)
        {
            // enough to buy launcher , tank
            if (rnd(100) < 50)
                BUILD_UNIT(LAUNCHER);
            else
                BUILD_UNIT(TANK);
        } 
        else if (player[ID].credits > units[TANK].cost)
        {
            BUILD_UNIT(TANK);
        }
    }

    if (iMission == 6)
    {
		// when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
		if (player[ID].credits > units[SIEGETANK].cost)
		{
			if (rnd(100) < 50)
				BUILD_UNIT(SIEGETANK);
			else
				BUILD_UNIT(LAUNCHER);
		}
		else if (player[ID].credits > units[TANK].cost)
		{
			if (rnd(100) < 30)
				BUILD_UNIT(TANK); // buy a normal tank in mission 6
		}
    }

    if (iMission == 7)
    {
        // when enough money, 50/50 on siege/launcher. Else just buy a tank or do nothing
		if (player[ID].credits > units[SIEGETANK].cost)
		{
			if (rnd(100) < 50)
				BUILD_UNIT(SIEGETANK);
			else
				BUILD_UNIT(LAUNCHER);
		}
		else if (player[ID].credits > units[TANK].cost)
		{
			if (rnd(100) < 30)
				BUILD_UNIT(TANK); // buy a normal tank in mission 6
		}
    }


}

void cAIPlayer::think_buildbase()
{
	if (game.bSkirmish)
	{
		if (player[ID].iStructures[CONSTYARD] > 0)
		{
            // already building
            for (int i=0; i < MAX_STRUCTURETYPES; i++)
                if (iBuildingStructure[i] > -1)
                {
                    // already building
                    return;
                }

			// when no windtrap, then build one (or when low power)
			if (player[ID].iStructures[WINDTRAP] < 1 || player[ID].bEnoughPower() == false)
			{
				BUILD_STRUCTURE(WINDTRAP);
				return;
			}

			// build refinery
			if (player[ID].iStructures[REFINERY] < 1)
			{
				BUILD_STRUCTURE(REFINERY);
				return;
			}

			// build wor / barracks
			if (player[ID].house == ATREIDES)
			{
				if (player[ID].iStructures[BARRACKS] < 1)
				{
					BUILD_STRUCTURE(BARRACKS);
					return;
				}
			}
			else 
			{
				if (player[ID].iStructures[WOR] < 1)
				{
					BUILD_STRUCTURE(WOR);
					return;
				}
			}

			if (player[ID].iStructures[RADAR] < 1)
			{
				BUILD_STRUCTURE(RADAR);
				return;
			}

			// from here, we can build turrets & rocket turrets

			if (player[ID].iStructures[LIGHTFACTORY] < 1)
			{
				BUILD_STRUCTURE(LIGHTFACTORY);
				return;
			}

			if (player[ID].iStructures[HEAVYFACTORY] < 1)
			{
				BUILD_STRUCTURE(HEAVYFACTORY);
				return;
			}

			// when mission is lower then 5, build normal turrets
			if (game.iMission <= 5)
			{
				if (player[ID].iStructures[TURRET] < 3)
				{
					BUILD_STRUCTURE(TURRET);
					return;
				}
			}

			if (game.iMission >= 6)
			{
				if (player[ID].iStructures[RTURRET] < 3)
				{
					BUILD_STRUCTURE(RTURRET);
					return;
				}
			}

            // build refinery
			if (player[ID].iStructures[REFINERY] < 2)
			{
				BUILD_STRUCTURE(REFINERY);
				return;
			}

			if (player[ID].iStructures[HIGHTECH] < 1)
			{
				BUILD_STRUCTURE(HIGHTECH);
				return;
			}

			if (player[ID].iStructures[REPAIR] < 1)
			{
				BUILD_STRUCTURE(REPAIR);
				return;
			}

			if (player[ID].iStructures[PALACE] < 1)
			{
				BUILD_STRUCTURE(PALACE);
				return;
			}
            
            if (game.iMission >= 6)
			{
				if (player[ID].iStructures[RTURRET] < 9)
				{
					BUILD_STRUCTURE(RTURRET);
					return;
				}
			}

   			if (player[ID].iStructures[STARPORT] < 1)
			{
				BUILD_STRUCTURE(STARPORT);
				return;
			}


		}

	}
	else
	{
		// rebuild only destroyed stuff
	}

}

void cAIPlayer::think_worm()
{

    // loop through all its worms and move them around
    for (int i=0; i < MAX_UNITS; i++)
        if (unit[i].isValid())
            if (unit[i].iType == SANDWORM && unit[i].iPlayer == ID)
            {
                // when on guard
                if (unit[i].iAction == ACTION_GUARD)
                {
                    // find new spot to go to
                    for (int iTries=0; iTries < 10; iTries++)
                    {
                        int iMoveTo = rnd(MAX_CELLS);

                        if (map.cell[iMoveTo].type == TERRAIN_SAND ||
                            map.cell[iMoveTo].type == TERRAIN_HILL ||
                            map.cell[iMoveTo].type == TERRAIN_SPICE ||
                            map.cell[iMoveTo].type == TERRAIN_SPICEHILL)
                        {
                            logbook("Order move #4");
                            UNIT_ORDER_MOVE(i, iMoveTo);
                            break;
                        }
                    }

                }
            }
}

/////////////////////////////////////////////////


int AI_STRUCTYPE(int iUnitType)
{
    // CHECK FOR BUILDING
    int iStrucType=HEAVYFACTORY;     // what do we need to build this unit?

    // Default = heavyfactory, so do a check if its NOT.

    // light vehicles
    if (iUnitType == TRIKE || iUnitType == RAIDER || iUnitType == QUAD)
        iStrucType =LIGHTFACTORY;
    
    
    // soldiers and troopers
    if (iUnitType == INFANTRY || iUnitType == SOLDIER)
        iStrucType = BARRACKS;

    if (iUnitType == TROOPER || iUnitType == TROOPERS)
        iStrucType = WOR;

    // airborn stuff
    if (iUnitType == CARRYALL || iUnitType == ORNITHOPTER)
        iStrucType = HIGHTECH;

    return iStrucType;
}

// Helper functions to keep fair play:
bool AI_UNITSTRUCTURETYPE(int iPlayer, int iUnitType)
{
    // This function will do a check what kind of structure is needed to build the unittype
    // Basicly the function returns true when its valid to build the unittype, or false
    // when its impossible (due no structure, money, etc)
    
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.

    // CHECK 1: Do we have the money?
    if (player[iPlayer].credits < units[iUnitType].cost)
        return false; // NOPE

    // CHECK 2: Aren't we building this already?
    if (aiplayer[iPlayer].iBuildingUnit[iUnitType] >= 0)
        return false;

    int iStrucType = AI_STRUCTYPE(iUnitType);

    // Do the reality-check, do we have the building needed?   
    if (player[iPlayer].iStructures[iStrucType] < 1)
        return false; // we do not have the building

    
    // WE MAY BUILD IT!
    return true;
}

int AI_RANDOM_UNIT_TARGET(int iPlayer, int iAttackPlayer)
{
	
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));
    
    int iT=0;

    for (int i=0; i < MAX_UNITS; i++)
        if (unit[i].isValid())
            if (unit[i].iPlayer == iAttackPlayer)
			{
				if (DEBUGGING)
				{
				char msg[255];
				sprintf(msg, "AI %d (House %d) -> Visible = %d", iPlayer, player[iPlayer].house, map.iVisible[unit[i].iCell][iPlayer]);
				logbook(msg);
				}

				if (map.iVisible[unit[i].iCell][iPlayer] || game.bSkirmish)
				{
					iTargets[iT] = i;
					iT++;

					if (iT > 99)
						break;                
				}
			}


	if (DEBUGGING)
	{
	char msg[255];
	sprintf(msg, "Targets %d", iT);
	logbook(msg);
	}

    return (iTargets[rnd(iT)]);
}

int AI_RANDOM_STRUCTURE_TARGET(int iPlayer, int iAttackPlayer)
{
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));
    
    int iT=0;

    for (int i=0; i < MAX_STRUCTURES; i++)
        if (structure[i])
            if (structure[i]->iPlayer == iAttackPlayer)
				if (map.iVisible[structure[i]->getCell()][iPlayer] || 
					game.bSkirmish)
				{
					iTargets[iT] = i;

					iT++;

					if (iT > 99)
						break;
				}


	if (DEBUGGING)
	{
	char msg[255];
	sprintf(msg, "STR] Targets %d", iT);
	logbook(msg);
	}


    return (iTargets[rnd(iT)]);
}

void cAIPlayer::think_repair_structure(cStructure *struc)
{
	// think of repairing, only when it is not being repaired yet.
	if (struc->bRepair == false) {
		// when ai has a lot of money, repair even faster
		if (player[struc->iPlayer].credits > 1000) {
			if (struc->getHitPoints() < (structures[struc->getType()].hp))  {
				struc->bRepair=true;
				logbook("AI Will repair structure");
			}
		} else {
			// AUTO-REPAIR BY AI
			if (struc->getHitPoints() < (structures[struc->getType()].hp/2)) {
				struc->bRepair=true;
				logbook("AI Will repair structure");
			}
		}
	}
}

int cAIPlayer::iPlaceStructureCell(int iType)
{
	// loop through all structures, and try to place structure 
	// next to them:
	//         ww 
	//           ss
	//           ss

	// s = structure:
	// w = start point (+ width of structure).
	
	// so starting at : x - width , y - height. 
	// ending at      : x + width of structure + width of type
	// ...            : y + height of s + height of type

	int iWidth=structures[iType].bmp_width/32;
	int iHeight=structures[iType].bmp_height/32;

	int iDistance=0;

	int iGoodCells[50]; // remember 50 possible locations
	int iGoodCellID=0;
				
	// clear out table of good locations
	memset(iGoodCells, -1, sizeof(iGoodCells));

	bool bGood=false;
	
	if (iCheckingPlaceStructure < 0)
		iCheckingPlaceStructure=0;

	if (iCheckingPlaceStructure >= MAX_STRUCTURES)
		iCheckingPlaceStructure=0;

	// loop through structures
	int i;
	for (i=iCheckingPlaceStructure; i < iCheckingPlaceStructure+2; i++)
	{
		// just in case
		if (i >= MAX_STRUCTURES)
			break;

		// valid
		if (structure[i])
		{
			// same owner
			if (structure[i]->iPlayer == ID)
			{
				// scan around
				int iStartX=iCellGiveX(structure[i]->getCell());
				int iStartY=iCellGiveY(structure[i]->getCell());

				int iEndX = iStartX + (structures[structure[i]->getType()].bmp_width/32) + 1;
				int iEndY = iStartY + (structures[structure[i]->getType()].bmp_height/32) + 1;

				iStartX -= iWidth;
				iStartY -= iHeight;

				// do not go out of boundries
				FIX_POS(iStartX, iStartY);
				FIX_POS(iEndX, iEndY);

				
				for (int sx=iStartX; sx < iEndX; sx++)
				{
					for (int sy=iStartY; sy < iEndY; sy++)
					{	
						int r =  cStructureFactory::getInstance()->getSlabStatus(iCellMake(sx, sy), iType, -1);

						if (r > -2)
						{
							if (iType != TURRET && iType != RTURRET)
							{
								// for turrets, the most far counts
								bGood=true;
								iGoodCells[iGoodCellID] = iCellMake(sx, sy);
								iGoodCellID++;                                
                                
                                return iCellMake(sx, sy);
							}
							else
							{
								bGood=true;

								int iDist=ABS_length(sx, sy, iCellGiveX(player[ID].focus_cell),iCellGiveY(player[ID].focus_cell));
														
								if ((iDist > iDistance) || (iDist == iDistance))
								{
									iDistance=iDist;
									iGoodCells[iGoodCellID] = iCellMake(sx, sy);
									iGoodCellID++;
                                    return iCellMake(sx, sy);
								}
							}
						}
					}
				} // sx
			} // same player
		} // valid structure
	}

	// not turret 
	if (bGood)
	{					
		//					STRUCTURE_CREATE(iGoodCells[rnd(iGoodCellID)], iType, structures[iType].hp, ID);
		logbook("FOUND CELL TO PLACE");
        iCheckingPlaceStructure=-1;
		return (iGoodCells[rnd(iGoodCellID)]);
	}

    iCheckingPlaceStructure=i;    
    player[ID].TIMER_think-=5;
    
	return -1;
}
