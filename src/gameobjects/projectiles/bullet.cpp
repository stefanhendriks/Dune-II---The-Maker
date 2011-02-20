/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "../../include/d2tmh.h"

#include <math.h>
void cBullet::init()
{
    bAlive=false;        // alive or not?, when dying, a particle is created

    iCell=-1;          // cell of bullet
    iType=-1;          // type of bullet

    // Movement
    iGoalCell=-1;      // the goal cell (goal of path)
    iOffsetX=-1;       // X offset
    iOffsetY=-1;       // Y offset

    iOwnerUnit=-1;     // unit who shoots
    iOwnerStructure=-1;// structure who shoots (rocket turret?)
    iPlayer = -1;

    iFrame=0;           // frame (for rockets)

    TIMER_frame=0;      // timer for switching frames

    iHoming=-1;        // homing to unit...
    TIMER_homing=0;   // when timer set, > 0 means homing

}


int cBullet::draw_x()
{
return ( (( iCellGiveX(iCell) * 32 ) - (mapCamera->getX()*32)) + iOffsetX);
}


int cBullet::draw_y()
{
return ( (( iCellGiveY(iCell) * 32 ) - (mapCamera->getY()*32)) + iOffsetY)+42;
}

// draw the bullet
void cBullet::draw()
{
    int x = draw_x();
    int y = draw_y();

    if (x < 0 || x > game.getScreenResolution()->getWidth())
        return;

    if (y < 0 || y > game.getScreenResolution()->getHeight())
        return;


    int x1, y1, x2, y2;
    x1 = iCellGiveX(iCell);
    y1 = iCellGiveY(iCell);
    x2 = iCellGiveX(iGoalCell);
    y2 = iCellGiveY(iGoalCell);

    int a  = bullet_face_angle(fDegrees(x1, y1, x2, y2));
    int fa = bullet_face_angle(fDegrees(x1, y1, x2, y2));
    int ba = bullet_correct_angle(fa);


    int h, w;
    w = bullets[iType].bmp_width;
    h = 32;

    int sx, sy;

    sy = iFrame * h;

    if (iType == ROCKET_SMALL ||
        iType == ROCKET_SMALL_FREMEN ||
        iType == ROCKET_SMALL_ORNI)
        sy = (iFrame *16);

    if (iType != BULLET_SMALL &&
        iType != BULLET_TRIKE &&
        iType != BULLET_QUAD &&
        iType != BULLET_TANK &&
        iType != BULLET_SIEGE &&
        iType != BULLET_DEVASTATOR &&
		iType != BULLET_TURRET)
      sx = ba * bullets[iType].bmp_width;
    else
      sx=0;


    // Whenever this bullet is a shimmer, draw a shimmer and leave
  if (iType == BULLET_SHIMMER)
  {
    Shimmer(16, x, y);
    return;
  }

  if (bullets[iType].bmp != NULL)
    masked_blit((BITMAP *)bullets[iType].bmp, bmp_screen, sx, sy, x, y, bullets[iType].bmp_width, bullets[iType].bmp_width);

  return;
}


void cBullet::think()
{

    // frame animation first
    TIMER_frame++;

    if (TIMER_frame > 12)
    {
        bool bCreatePuf=false;

        // big rockets create smoke
        if (iType == ROCKET_NORMAL || iType == BULLET_GAS || iType == ROCKET_RTURRET)
        {
            iFrame++;
            if (iFrame > 1)
                iFrame=0;

            bCreatePuf=true;
        }


        // smaller rockets don't
        if (iType == ROCKET_SMALL || iType == ROCKET_SMALL_ORNI)
        {
            iFrame++;
            if (iFrame > 1)
                iFrame=0;

            if (iType == ROCKET_SMALL_ORNI)
                bCreatePuf=true;

        }

        // except for orni's



        if (bCreatePuf)
            PARTICLE_CREATE(draw_x()+(mapCamera->getX()*32)+16 , draw_y()+(mapCamera->getY()*32)+16, BULLET_PUF, -1, -1);

        TIMER_frame = 0;

    }

    // when this bastard is homing... set goal
    if (TIMER_homing > 0)
    {
        TIMER_homing--;

        if (iHoming > -1)
            if (unit[iHoming].isValid())
                iGoalCell = unit[iHoming].iCell;

        // this units gonna die!
    }

    think_move();
}


void cBullet::think_move()
{
// now move
    int cx = iCellGiveX(iGoalCell);
    int cy = iCellGiveY(iGoalCell);
    int iCellX = iCellGiveX(iCell);
    int iCellY = iCellGiveY(iCell);

    int iSlowDown=0;

    // use this when picking something up

    // step 1 : look to the correct direction
    int d = fDegrees(iCellX, iCellY, cx, cy);
    float angle = fRadians(iCellX, iCellY, cx, cy);

    // now do some thing to make
    // 1/8 of a cell (2 pixels) per movement
    iOffsetX += cos(angle)*2;
    iOffsetY += sin(angle)*2;

    bool update_me=false;
    // update when to much on the right.
    if (iOffsetX > 31)
    {
        iOffsetX -= 32;
        iCell++;
        update_me=true;
    }

    // update when to much on the left
    if (iOffsetX < -31)
    {
        iOffsetX += 32;
        iCell--;
        update_me=true;
    }

                  // update when to much up
    if (iOffsetY < -31)
    {
        iOffsetY += 32;
        iCell -= MAP_W_MAX;
        update_me=true;
    }

    // update when to much down
    if (iOffsetY > 31)
    {
        iOffsetY -= 32;
        iCell += MAP_W_MAX;
        update_me=true;
    }

    if (iCell==iGoalCell)
        iOffsetX=iOffsetY=0;

    if (update_me)
    {
        if (bCellValid(iCell) == false)
        {
            if (iCell > (MAX_CELLS-1)) iCell = MAX_CELLS-1;
            if (iCell < 0) iCell = 0;
        }
    }

    bool bDie=false;
    bool bDamageRockets=true;

    cPlayerDifficultySettings * difficultySettings = player[iPlayer].getDifficultySettings();

    // crash against mountains, etc
    if (iType == BULLET_SMALL ||
        iType == BULLET_TRIKE ||
        iType == BULLET_QUAD ||
        iType == BULLET_TANK ||
        iType == BULLET_SIEGE ||
        iType == BULLET_DEVASTATOR ||
        iType == BULLET_TURRET) {
        bDamageRockets=false;

        // hit structures , walls and mountains
        if (map.cell[iCell].type == TERRAIN_MOUNTAIN)
            bDie=true;

        if (map.cell[iCell].type == TERRAIN_WALL && iType != BULLET_TURRET) // except for bullet turret that do not hit walls...
        {
            // damage this type of wall...
			int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

			if (iOwnerUnit > -1)
			{
				int iDam=(unit[iOwnerUnit].fExpDamage() * iDamage);
				iDamage += iDam;
			}

            //map.cell[iCell].health -= player[iPlayer].iDamage(bullets[iType].damage);
			map.cell[iCell].health -= iDamage;

            if (map.cell[iCell].health < 0)
            {
                // remove wall, turn into smudge:
                mapEditor.createCell(iCell, TERRAIN_ROCK, 0);
                mapEditor.smoothAroundCell(iCell);
                map.smudge_increase(SMUDGE_WALL, iCell);

            }

            bDie=true;
        }


        if (map.cell[iCell].id[MAPID_STRUCTURES] > -1) {

            // structure hit!
            int id = map.cell[iCell].id[MAPID_STRUCTURES];

            bool bSkipSelf=false;

            if (iType == BULLET_TURRET)
            {
				if (id == iOwnerStructure) {
					//logbook("Turret bullet shot itself, will skip friendly fire");
                    bSkipSelf=true; // do not shoot yourself
				} else {
					if (structure[id]->getOwner() == iPlayer) {
                        bSkipSelf=true; // do not shoot own buildings
						//logbook("Bullet shot itself, will skip friendly fire");
					}
                }
            }

            if (bSkipSelf == false) {
				int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

				// increase damage by experience of unit
				if (iOwnerUnit > -1) {
					// extra damage by experience:
					int iDam = (unit[iOwnerUnit].fExpDamage() * iDamage);
					iDamage = iDamage + iDam;
				}

				int oldHp = structure[id]->getHitPoints();
				assert(iDamage > -1); // as long not giving health it is fine. (0 too!)
				structure[id]->damage(iDamage);

				// this assert is disabled, because it is not obliged to have bullets that cause damage
				// think of the deviator missiles.
				//assert(oldHp >= structure[id]->getHitPoints()); // damage should be done

				int iChance = 10;

				if (structure[id]->getHitPoints() < (structures[structure[id]->getType()].hp / 2))
					iChance = 30;

				if (rnd(100) < iChance)
					PARTICLE_CREATE(draw_x()+(mapCamera->getX()*32)+16+ (-8 + rnd(16)), draw_y()+(mapCamera->getY()*32)+16+ (-8 + rnd(16)), OBJECT_SMOKE, -1, -1);


				// NO HP LEFT, DIE
				if (structure[id]->getHitPoints() <= 0)
				{
					if (iOwnerUnit > -1)
						if (unit[iOwnerUnit].isValid())
						{
							// TODO: update statistics
//							player[unit[iOwnerUnit].iPlayer].iKills[INDEX_KILLS_STRUCTURES]++;  // we killed!
						}

						structure[id]->die();
				}

				bDie=true;
            } // skip self
			else {
				logbook("Skipped friendly fire");
			}

        }

    }

    if (iCell == iGoalCell || bDie)
    {
        // for non bullets (thus rockets!)
        if (bDamageRockets)
        {
			if (map.cell[iCell].id[MAPID_STRUCTURES] > -1)
			{
				// structure hit!
				int id = map.cell[iCell].id[MAPID_STRUCTURES];

				int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

				if (iOwnerUnit > -1)
				{
					int iDam = (unit[iOwnerUnit].fExpDamage() * iDamage);
					iDamage += iDam;
				}

				// this assertion is false, some bullets (like bullet_gas) do not
				// any damage.
//				assert(iDamage > 0);
				if (iDamage > 0) {
					structure[id]->damage(iDamage);

					int iChance=15;

					// structure could be dead here (damage->calls die when dead)
					if (structure && structure[id]->getHitPoints() < (structures[structure[id]->getType()].hp / 2))
						iChance = 45;

					// smoke
					if (rnd(100) < iChance)
						PARTICLE_CREATE(draw_x()+(mapCamera->getX()*32)+16+ (-8 + rnd(16)), draw_y()+(mapCamera->getY()*32)+16+ (-8 + rnd(16)), OBJECT_SMOKE, -1, -1);


					// NO HP LEFT, DIE
					if (structure[id]->getHitPoints() <= 0)
					{
						if (iOwnerUnit > -1)
							if (unit[iOwnerUnit].isValid())
							{
								// TODO: update statistics
//								player[unit[iOwnerUnit].iPlayer].iKills[INDEX_KILLS_STRUCTURES]++;  // we killed!
							}


							structure[id]->die();
					}

					bDie=true;
				}
			}

        if (map.cell[iCell].type == TERRAIN_WALL)
        {
            // damage this type of wall...
			int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

			if (iOwnerUnit > -1)
			{
				int iDam = (unit[iOwnerUnit].fExpDamage() * iDamage);
				iDamage += iDam;
			}

            map.cell[iCell].health -= iDamage;

            if (map.cell[iCell].health < 0)
            {
                // remove wall, turn into smudge:
            	mapEditor.createCell(iCell, TERRAIN_ROCK, 0);
                mapEditor.smoothAroundCell(iCell);
                map.smudge_increase(SMUDGE_WALL, iCell);

            }

            bDie=true;
        }

        } // do damage by rockets or not?

        // SPICE BLOOM
        if (map.cell[iCell].type == TERRAIN_BLOOM)
		{
			// change type of terrain to sand
			mapEditor.createCell(iCell, TERRAIN_SAND, 0);

			mapEditor.createField(iCell, TERRAIN_SPICE, 50+(rnd(75)));

			// kill unit
			game.TIMER_shake=20;

        }

		// Ok sandworm damaged
		if (map.cell[iCell].id[MAPID_WORMS] > -1)
        {
            // structure hit!
            int id = map.cell[iCell].id[MAPID_WORMS];

            unit[id].iHitPoints -= difficultySettings->getInflictDamage(bullets[iType].damage);

            // NO HP LEFT, DIE
            if (unit[id].iHitPoints <= 0)
            {
                unit[id].die(true, false);
            }

            bDie=true;
        }

        // OK, units damaged:
        if (map.cell[iCell].id[MAPID_UNITS] > -1)
        {
            // structure hit!
            int id = map.cell[iCell].id[MAPID_UNITS];

            if (units[unit[id].iType].infantry)
			{
				int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage_inf);

				if (iOwnerUnit > -1)
				{
					int iDam = unit[iOwnerUnit].fExpDamage() * iDamage;
					iDamage = iDamage + iDam;
				}

	            unit[id].iHitPoints -= iDamage;
			}
			else
			{
				int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

				if (iOwnerUnit > -1)
				{
					int iDam = unit[iOwnerUnit].fExpDamage() * iDamage;
					iDamage = iDamage + iDam;
				}

	            unit[id].iHitPoints -= iDamage;
				//unit[id].iHitPoints -= player[iPlayer].iDamage(bullets[iType].damage);

			}


            // NO HP LEFT, DIE
            if (unit[id].iHitPoints <= 0)
            {
               int iID = iOwnerUnit;

               if (iID < 0)
                   iID = iOwnerStructure;

               if (iID > -1)
               {
				   if (iOwnerUnit > -1)
				   {
                    if (unit[iID].isValid())
                    {
						// TODO: update statistics
//                        player[unit[iID].iPlayer].iKills[INDEX_KILLS_UNITS]++;  // we killed!

						if (units[unit[id].iType].infantry)
							unit[iID].fExperience += 0.25; // 4 kills = 1 star
						else
							unit[iID].fExperience += 0.45; // ~ 3 kills = 1 star
                    }
				   }


               }

               unit[id].die(true, false);
            }
            else
                unit[id].think_hit(iOwnerUnit, iOwnerStructure); // this unit will think what to do now (he got hit ouchy!)


            bDie=true;

            if (iType == BULLET_GAS)
            {
				if (rnd(100) < 40)
				{
                    if (iOwnerUnit > -1) {
                        unit[id].iPlayer = unit[iOwnerUnit].iPlayer;
                    } else if (iOwnerStructure > -1) {
                        unit[id].iPlayer = structure[iOwnerStructure]->getOwner();
					}

                    unit[id].iAttackStructure = -1;
                    unit[id].iAttackUnit=-1;
                    unit[id].iAction = ACTION_GUARD;
				}
            }

        }


        // ok, only rockets can damage aircraft:
        if (iType == ROCKET_NORMAL ||
            iType == ROCKET_SMALL_ORNI ||
            iType == ROCKET_SMALL ||
            iType == ROCKET_SMALL_FREMEN ||
            iType == ROCKET_RTURRET)
        {
            if (map.cell[iCell].id[MAPID_AIR] > -1)
            {
            // structure hit!
            int id = map.cell[iCell].id[MAPID_AIR];

            if (id != iOwnerUnit)
            {

				int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

				if (iOwnerUnit > -1)
				{
					int iDam = (unit[iOwnerUnit].fExpDamage() * iDamage);

					iDamage += iDam;
				}

	            unit[id].iHitPoints -= iDamage;

//            unit[id].iHitPoints -= player[iPlayer].iDamage(bullets[iType].damage);

            // NO HP LEFT, DIE
            if (unit[id].iHitPoints <= 0)
            {
                unit[id].die(true, false);

                int iID = iOwnerUnit;

                if (iID < 0)
                    iID = iOwnerStructure;

               if (iID > -1)
               {
                    if (unit[iID].isValid())
                    {
						// TODO: update statistics
//                        player[unit[iID].iPlayer].iKills[INDEX_KILLS_UNITS]++;  // we killed!
                    }
               }


            }

            bDie=true;
            }
        }

        }


        if (iType == BULLET_GAS)
            play_sound_id(SOUND_GAS, iCellOnScreen(iCell));

        if (bullets[iType].deadbmp > -1)
            PARTICLE_CREATE(draw_x()+(mapCamera->getX()*32)+16+ (-8 + rnd(16)), draw_y()+(mapCamera->getY()*32)+16+ (-8 + rnd(16)), bullets[iType].deadbmp, -1, -1);

		int iDamage = difficultySettings->getInflictDamage(bullets[iType].damage);

		if (iOwnerUnit > -1)
		{
			int iDam = (unit[iOwnerUnit].fExpDamage() * iDamage);
			iDamage += iDam;
		}

        if (map.cell[iCell].type == TERRAIN_ROCK)
        {
            if (map.cell[iCell].type != TERRAIN_WALL)
                map.cell[iCell].health -= iDamage;


            if (map.cell[iCell].health < -25)
            {
                map.smudge_increase(SMUDGE_ROCK, iCell);
                map.cell[iCell].health += rnd(25);
            }
        }
		else if (map.cell[iCell].type == TERRAIN_SLAB)
		{
			// change into rock, get destroyed
			if (map.cell[iCell].id[MAPID_STRUCTURES] < 0 &&
				iType != BULLET_SMALL &&
				iType != BULLET_SHIMMER)
			{
				map.cell[iCell].type = TERRAIN_ROCK;
				mapEditor.smoothAroundCell(iCell);
			}
		}
        else if (map.cell[iCell].type == TERRAIN_SAND ||
                 map.cell[iCell].type == TERRAIN_HILL ||
                 map.cell[iCell].type == TERRAIN_SPICE ||
                 map.cell[iCell].type == TERRAIN_SPICEHILL)
        {
            if (map.cell[iCell].type != TERRAIN_WALL)
                map.cell[iCell].health -= iDamage;


            if (map.cell[iCell].health < -25)
            {
                map.smudge_increase(SMUDGE_SAND, iCell);
                map.cell[iCell].health += rnd(25);
            }
        }

        bAlive=false;
    }

}
