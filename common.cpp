/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"
#include <math.h>

// Keep a logbook
void logbook(char *txt) 
{
  FILE *fp;
  fp = fopen("log.txt", "at");
    
  if (fp) 
  {
    fprintf(fp, "%d| %s\n", game.iGameTimer, txt); // print the text into the file
  }

  fclose(fp);
}

// determine if this cell is not out of boundries
bool BORDER_POS(int x, int y)
{	
	if (x < 1) return false;
    if (x > (game.map_width-1)) return false;

    if (y < 1) return false;
    if (y > (game.map_height-1)) return false;

	return true; // the fix-border-pos function did not change/correct the positions! yay
}

// fixes the positions according to the PLAYABLE size of the map (for unit
// dumping, etc)
void FIX_BORDER_POS(int &x, int &y)
{
	// filled in
    if (x)
    {
        if (x < 1) x = 1;
        if (x > (game.map_width-1)) x = (game.map_width-1);
    }

    // filled in
    if (y)
    {
        if (y < 1) y = 1;
        if (y > (game.map_height-1)) y = (game.map_height-1);
    }
}

// Will make sure the X and Y don't get out of their boundries
void FIX_POS(int &x, int &y)
{
    // filled in
    if (x)
    {
        if (x < 0) x = 0;
        if (x > game.map_width) x = game.map_width;
    }

    // filled in
    if (y)
    {
        if (y < 0) y = 0;
        if (y > game.map_height) y = game.map_height;
    }
}




void INSTALL_PLAYERS()
{
	for (int i=0; i < MAX_PLAYERS; i++)
		player[i].init();
}



void INSTALL_WORLD()
{
    // create regions
    for (int i=0; i < MAX_REGIONS; i++)
    {
        world[i].bSelectable=false;
        world[i].iAlpha=0;
        world[i].iHouse=-1;
        world[i].iTile;
        world[i].x=-1;
        world[i].y=-1;

    }

    // Now create the regions (x,y wise)
    
    // FIRST ROW (EASY, SAME Y AXIS)
    REGION_NEW(16 ,  73, 1, -1, PIECE_DUNE_001);
    REGION_NEW(126,  73, 1, -1, PIECE_DUNE_002);
    REGION_NEW(210,  73, 1, -1, PIECE_DUNE_003);
    REGION_NEW(306,  73, 1, -1, PIECE_DUNE_004);
    REGION_NEW(438,  73, 1, -1, PIECE_DUNE_005);
    REGION_NEW(510,  73, 1, -1, PIECE_DUNE_006);

    // SECOND ROW, HARDER FROM NOW ON (DIFFERENT Y AXIS ALL THE TIME)
    REGION_NEW(16,  91, 1, -1, PIECE_DUNE_007);
    REGION_NEW(158,  149, 1, -1, PIECE_DUNE_008);
    REGION_NEW(282,  135, 1, -1, PIECE_DUNE_009);
    REGION_NEW(362,  105, 1, -1, PIECE_DUNE_010);
    REGION_NEW(456,  117, 1, -1, PIECE_DUNE_011);
    REGION_NEW(544,  105, 1, -1, PIECE_DUNE_012);

    // THIRD ROW
    REGION_NEW(16,  155, 1, -1, PIECE_DUNE_013);
    REGION_NEW(58,  165, 1, -1, PIECE_DUNE_014);
    REGION_NEW(190,  213, 1, -1, PIECE_DUNE_015);
    REGION_NEW(312,  163, 1, -1, PIECE_DUNE_016);
    REGION_NEW(388,  163, 1, -1, PIECE_DUNE_017);
    REGION_NEW(502,  167, 1, -1, PIECE_DUNE_018);
    REGION_NEW(576,  167, 1, -1, PIECE_DUNE_019);

    // FOURTH ROW
    REGION_NEW(16,  237, 1, -1, PIECE_DUNE_020);
    REGION_NEW(62,  255, 1, -1, PIECE_DUNE_021);
    REGION_NEW(134,  245, 1, -1, PIECE_DUNE_022);
    REGION_NEW(282,  257, 1, -1, PIECE_DUNE_023);
    REGION_NEW(360,  253, 1, -1, PIECE_DUNE_024);
    REGION_NEW(406,  213, 1, -1, PIECE_DUNE_025);
    REGION_NEW(448,  269, 1, -1, PIECE_DUNE_026);
    REGION_NEW(514,  227, 1, -1, PIECE_DUNE_027);

  //  game.iHouse = ATREIDES;
    //REGION_SETUP(4, game.iHouse);

    
}

void INSTALL_HOUSES()
{
/********************************
 House Rules
 ********************************/


  // General / Default / No House
  houses[GENERALHOUSE].swap_color   = 128;
  houses[GENERALHOUSE].minimap_color = makecol(128,128,128);

  // Harkonnen
  houses[HARKONNEN].swap_color = -1;  // 144
  houses[HARKONNEN].minimap_color = makecol(125,0,0);
    
  // Atreides
  houses[ATREIDES].swap_color  = 160;
  houses[ATREIDES].minimap_color = makecol(24,32,125);

  // Ordos
  houses[ORDOS].swap_color     = 176;
  houses[ORDOS].minimap_color = makecol(24,125,24);

  // Mercenary
  houses[MERCENARY].swap_color    = 192;
  houses[MERCENARY].minimap_color = makecol(214, 121, 16);

  // Sardaukar
  houses[SARDAUKAR].swap_color = 208;
  houses[SARDAUKAR].minimap_color = makecol(137,24,137);

  // Fremen
  houses[FREMEN].swap_color = 224;
  houses[FREMEN].minimap_color = makecol(214,149,0);

  // GREY

  // Corrino (?)
  houses[CORRINO].swap_color   = 136;
  houses[FREMEN].minimap_color = makecol(192,192,192); // grey


  // TEMP
  //map.randommap();
}


bool MOUSE_BTN_LEFT()
{
  if (mouse_b & 1)
    return true; // return true, no check for 'already pressed'

  return false;
}

// Did we press the right mouse button? (instant)
bool MOUSE_BTN_RIGHT()
{
  if (mouse_b & 2)
      return true;    // yeap    
  
  return false;
}


// Did we press the left mouse button?
bool mouse_pressed_left()
{
  if (MOUSE_BTN_LEFT())
  {
    // not yet pressed
    if (game.mouse_left == false)
    {
      // we pressed this just for once
      game.mouse_left=true;
      return true;    // yeap
    }
  }
  else
  {
    game.mouse_left=false;
    return false;   // no way
  }

  return false;
}

// Did we press the right mouse button?
bool mouse_pressed_right()
{
  if (MOUSE_BTN_RIGHT())
  {
    if (game.mouse_right == false)
    {
      game.mouse_right = true;
      return true;    // yeap
    }
  }
  else
  {
    game.mouse_right = false;
    return false;   // no way
  }

  return false;
}
/*****************************
 Unit Rules
 *****************************/
void install_units()
{  
  // Every unit thinks at 0.1 second. When the unit thinks, it is thinking about the path it
  // is taking, the enemies around him, etc. The speed of how a unit should move is depended on
  // time aswell. Every 0.01 second a unit 'can' move. The movespeed is like this:
  // 0    - slowest (1 second per pixel)
  // 1000 - fastest (1 pixel per 0.01 second)
  // So, the higher the number, the faster it is.


  // some things for ALL unit types; initialization
  for (int i = 0; i < MAX_UNITTYPES; i++)
  {
    units[i].bmp              = (BITMAP *)gfxdata[UNIT_QUAD].dat; // in case an invalid unit is choosen, it is a quad! :D
    units[i].top              = NULL;  // no top
    units[i].shadow           = NULL;  // no shadow (deliverd with picture itself)
    units[i].bmp_width        = 0;
    units[i].bmp_height       = 0;
    units[i].turnspeed        = 0;
    units[i].speed            = 0;
    units[i].icon             = -1;
    units[i].hp               = -1;
    units[i].bullets          = -1;
    units[i].attack_frequency = -1;
    units[i].build_time       = -1;
    units[i].airborn          = false;
    units[i].infantry         = false;
    units[i].free_roam        = false;
    units[i].second_shot      = false;
    units[i].squish           = true;     // most units can squish
    units[i].range            = -1;
    units[i].sight            = -1;

    // harvester properties
    units[i].harvesting_amount= 0;
    units[i].harvesting_speed = 0;
    units[i].credit_capacity  = 0;
    
    strcpy(units[i].name, "\0");
  }

  // Unit        : CarryAll 
  // Description : CarryAll, the flying pickuptruck
  units[CARRYALL].bmp = (BITMAP *)gfxdata[UNIT_CARRYALL].dat;      // pointer to the original 8bit bitmap
  units[CARRYALL].shadow = (BITMAP *)gfxdata[UNIT_CARRYALL_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[CARRYALL].bmp_width  = 24*2;
  units[CARRYALL].bmp_height = 24*2;
  units[CARRYALL].bmp_startpixel = 0;
  units[CARRYALL].bmp_frames = 2; // we have at max 1 extra frame
  units[CARRYALL].icon = ICON_UNIT_CARRYALL;
  units[CARRYALL].airborn=true;   // is airborn
  units[CARRYALL].free_roam=true; // may freely roam the air
  strcpy(units[CARRYALL].name, "Carry-All");

      // Unit        : Ornithopter 
  // Description : Pesty little aircraft shooting bastard
  units[ORNITHOPTER].bmp = (BITMAP *)gfxdata[UNIT_ORNITHOPTER].dat;      // pointer to the original 8bit bitmap
  units[ORNITHOPTER].shadow = (BITMAP *)gfxdata[UNIT_ORNITHOPTER_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[ORNITHOPTER].bmp_width  = 24*2;
  units[ORNITHOPTER].bmp_height = 24*2;
  units[ORNITHOPTER].bmp_startpixel = 0;
  units[ORNITHOPTER].bmp_frames = 4; // we have at max 3 extra frames
  units[ORNITHOPTER].icon = ICON_UNIT_ORNITHOPTER;
  units[ORNITHOPTER].bullets = ROCKET_SMALL_ORNI;
  units[ORNITHOPTER].second_shot = true;
  units[ORNITHOPTER].airborn = true;   // is airborn
  units[ORNITHOPTER].free_roam=true; // may freely roam the air  
  strcpy(units[ORNITHOPTER].name, "Ornithopter");

    // Unit        : Devastator
  // Description : Devastator
  units[DEVASTATOR].bmp = (BITMAP *)gfxdata[UNIT_DEVASTATOR].dat;      // pointer to the original 8bit bitmap  
  units[DEVASTATOR].shadow = (BITMAP *)gfxdata[UNIT_DEVASTATOR_SHADOW].dat;      // pointer to the original bitmap shadow
  units[DEVASTATOR].bmp_width  = 19*2;
  units[DEVASTATOR].bmp_startpixel = 0;
  units[DEVASTATOR].bmp_height = 23*2;
  units[DEVASTATOR].bmp_frames = 1;
  units[DEVASTATOR].bullets = BULLET_DEVASTATOR;
  units[DEVASTATOR].second_shot = true;
  units[DEVASTATOR].icon    = ICON_UNIT_DEVASTATOR;
  strcpy(units[DEVASTATOR].name, "Devastator");

  // Unit        : Harvester
  // Description : Harvester 
  units[HARVESTER].bmp = (BITMAP *)gfxdata[UNIT_HARVESTER].dat;      // pointer to the original 8bit bitmap  
  units[HARVESTER].shadow = (BITMAP *)gfxdata[UNIT_HARVESTER_SHADOW].dat;      // pointer to the original 8bit bitmap  
  units[HARVESTER].bmp_width  = 40*2;
  units[HARVESTER].bmp_startpixel = 24;
  units[HARVESTER].bmp_height = 26*2;
  units[HARVESTER].bmp_frames = 4;
  units[HARVESTER].icon = ICON_UNIT_HARVESTER;
  units[HARVESTER].credit_capacity = 700;
  units[HARVESTER].harvesting_amount = 5;
  strcpy(units[HARVESTER].name, "Harvester");

  // Unit        : Combattank
  // Description : Combattank
  units[TANK].bmp = (BITMAP *)gfxdata[UNIT_TANKBASE].dat;      // pointer to the original 8bit bitmap  
  units[TANK].shadow = (BITMAP *)gfxdata[UNIT_TANKBASE_SHADOW].dat;      // pointer to the original 8bit bitmap  
  units[TANK].top = (BITMAP *)gfxdata[UNIT_TANKTOP].dat;      // pointer to the original 8bit bitmap  
  units[TANK].bmp_width  = 16*2;
  units[TANK].bmp_startpixel = 0;
  units[TANK].bmp_height = 16*2;
  units[TANK].bmp_frames = 0;
  units[TANK].bullets = BULLET_TANK;
  units[TANK].icon    = ICON_UNIT_TANK;
  strcpy(units[TANK].name, "Tank");


  // Unit        : Siege Tank
  // Description : Siege tank
  units[SIEGETANK].bmp = (BITMAP *)gfxdata[UNIT_SIEGEBASE].dat;      // pointer to the original 8bit bitmap  
  units[SIEGETANK].shadow = (BITMAP *)gfxdata[UNIT_SIEGEBASE_SHADOW].dat;      // pointer to the original 8bit bitmap  
  units[SIEGETANK].top = (BITMAP *)gfxdata[UNIT_SIEGETOP].dat;      // pointer to the original 8bit bitmap  
  units[SIEGETANK].bmp_width  = 18*2;
  units[SIEGETANK].bmp_startpixel = 1;
  units[SIEGETANK].bmp_height = 18*2;
  units[SIEGETANK].bmp_frames = 0;
  units[SIEGETANK].bullets = BULLET_SIEGE;
  units[SIEGETANK].second_shot = true;
  units[SIEGETANK].icon    = ICON_UNIT_SIEGETANK;
  strcpy(units[SIEGETANK].name, "Siege Tank");

  // Unit        : MCV
  // Description : Movable Construction Vehicle  
  units[MCV].bmp = (BITMAP *)gfxdata[UNIT_MCV].dat;      // pointer to the original 8bit bitmap  
  units[MCV].shadow = (BITMAP *)gfxdata[UNIT_MCV_SHADOW].dat;      // pointer to the original 8bit bitmap  
  units[MCV].bmp_width  = 24*2;
  units[MCV].bmp_startpixel = 0;
  units[MCV].bmp_height = 25*2;
  units[MCV].bmp_frames = 1;
  units[MCV].icon = ICON_UNIT_MCV;
  strcpy(units[MCV].name, "MCV");


  // Unit        : Deviator
  // Description : Deviator
  units[DEVIATOR].bmp = (BITMAP *)gfxdata[UNIT_DEVIATOR].dat;      // pointer to the original 8bit bitmap
  units[DEVIATOR].bmp_width  = 16*2;
  units[DEVIATOR].bmp_height = 16*2;
  units[DEVIATOR].bmp_startpixel = 0;
  units[DEVIATOR].bmp_frames = 1;  
  units[DEVIATOR].icon = ICON_UNIT_DEVIATOR;
  units[DEVIATOR].bullets = BULLET_GAS; // our gassy rocket
  strcpy(units[DEVIATOR].name, "Deviator");

  // Unit        : Launcher
  // Description : Rocket Launcher
  units[LAUNCHER].bmp = (BITMAP *)gfxdata[UNIT_LAUNCHER].dat;      // pointer to the original 8bit bitmap  
  units[LAUNCHER].shadow = (BITMAP *)gfxdata[UNIT_LAUNCHER_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[LAUNCHER].bmp_width  = 16*2;
  units[LAUNCHER].bmp_height = 16*2;
  units[LAUNCHER].bmp_startpixel = 0;
  units[LAUNCHER].bmp_frames = 1;  
  units[LAUNCHER].icon = ICON_UNIT_LAUNCHER;
  units[LAUNCHER].second_shot = true;
  //units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
  units[LAUNCHER].bullets = ROCKET_NORMAL; // our gassy rocket
  strcpy(units[LAUNCHER].name, "Launcher");

  // Unit        : Quad
  // Description : Quad, 4 wheeled (double gunned)
  units[QUAD].bmp = (BITMAP *)gfxdata[UNIT_QUAD].dat;      // pointer to the original 8bit bitmap
  units[QUAD].shadow = (BITMAP *)gfxdata[UNIT_QUAD_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[QUAD].bmp_width  = 16*2;
  units[QUAD].bmp_height = 16*2;
  units[QUAD].bmp_startpixel = 0;
  units[QUAD].bmp_frames = 1;
  units[QUAD].icon = ICON_UNIT_QUAD;
  units[QUAD].second_shot = true;
  units[QUAD].bullets = BULLET_QUAD;
  units[QUAD].squish=false;
  strcpy(units[QUAD].name, "Quad");


  // Unit        : Trike (normal trike)
  // Description : Trike, 3 wheeled (single gunned)
  units[TRIKE].bmp = (BITMAP *)gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
  units[TRIKE].shadow = (BITMAP *)gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[TRIKE].bmp_width  = 14*2;
  units[TRIKE].bmp_height = 14*2;
  units[TRIKE].bmp_startpixel = 0;
  units[TRIKE].bmp_frames = 1;
  units[TRIKE].icon = ICON_UNIT_TRIKE;
  units[TRIKE].bullets = BULLET_TRIKE;   
  units[TRIKE].squish=false;
  strcpy(units[TRIKE].name, "Trike");

  // Unit        : Raider Trike (Ordos trike)
  // Description : Raider Trike, 3 wheeled (single gunned), weaker, but faster
  units[RAIDER].bmp = (BITMAP *)gfxdata[UNIT_TRIKE].dat;      // pointer to the original 8bit bitmap
  units[RAIDER].shadow = (BITMAP *)gfxdata[UNIT_TRIKE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[RAIDER].bmp_width  = 14*2;
  units[RAIDER].bmp_height = 14*2;
  units[RAIDER].bmp_startpixel = 0;
  units[RAIDER].bmp_frames = 1;
  strcpy(units[RAIDER].name, "Raider Trike");
  units[RAIDER].icon = ICON_UNIT_RAIDER;
  units[RAIDER].bullets = BULLET_TRIKE;
  units[RAIDER].squish=false;
  
  

  // Unit        : Frigate
  // Description : Frigate
  units[FRIGATE].bmp = (BITMAP *)gfxdata[UNIT_FRIGATE].dat;      // pointer to the original 8bit bitmap
  units[FRIGATE].shadow = (BITMAP *)gfxdata[UNIT_FRIGATE_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[FRIGATE].bmp_width  = 32*2;
  units[FRIGATE].bmp_height = 32*2;
  units[FRIGATE].bmp_startpixel = 0;
  units[FRIGATE].bmp_frames = 2; // we have at max 1 extra frame
  units[FRIGATE].speed = 2;  
  units[FRIGATE].turnspeed = 40;
  units[FRIGATE].airborn = true;
  units[FRIGATE].squish = false;
  units[FRIGATE].free_roam = true; // Frigate does not roam, yet needed?
  units[FRIGATE].hp = 9999;
  strcpy(units[FRIGATE].name, "Frigate");

  /*
  units[FRIGATE].speed     = 0;
  units[FRIGATE].turnspeed = 1;

  units[FRIGATE].sight      = 3;
  units[FRIGATE].hp         = 9999;
  */

  //units[FRIGATE].icon = ICON_FRIGATE;

  // Unit        : Sonic Tank
  // Description : Sonic tank (using sound waves to destroy)
  units[SONICTANK].bmp = (BITMAP *)gfxdata[UNIT_SONICTANK].dat;      // pointer to the original 8bit bitmap
  units[SONICTANK].shadow = (BITMAP *)gfxdata[UNIT_SONICTANK_SHADOW].dat;      // pointer to the original 8bit bitmap
  units[SONICTANK].bmp_width  = 16*2;
  units[SONICTANK].bmp_height = 16*2;
  units[SONICTANK].bmp_startpixel = 0;
  units[SONICTANK].bmp_frames = 1; // no extra frames
  units[SONICTANK].bullets = BULLET_SHIMMER;
  units[SONICTANK].icon = ICON_UNIT_SONICTANK;
  strcpy(units[SONICTANK].name, "Sonic Tank");

 
  // Unit        : Single Soldier
  // Description : 1 soldier
  units[SOLDIER].bmp = (BITMAP *)gfxdata[UNIT_SOLDIER].dat;      // pointer to the original 8bit bitmap
  units[SOLDIER].bmp_width  = 16*2;
  units[SOLDIER].bmp_height = 16*2;
  units[SOLDIER].bmp_startpixel = 0;
  units[SOLDIER].bmp_frames = 3; // 2 extra frames
  units[SOLDIER].infantry=true;  
  units[SOLDIER].bullets  = BULLET_SMALL;
  units[SOLDIER].icon       = ICON_UNIT_SOLDIER;
  units[SOLDIER].squish=false;
  strcpy(units[SOLDIER].name, "Soldier");
  
  
  // Unit        : Infantry
  // Description : 3 soldiers
  units[INFANTRY].bmp = (BITMAP *)gfxdata[UNIT_SOLDIERS].dat;      // pointer to the original 8bit bitmap
  units[INFANTRY].bmp_width  = 16*2;
  units[INFANTRY].bmp_height = 16*2;
  units[INFANTRY].bmp_startpixel = 0;
  units[INFANTRY].bmp_frames = 3; // 2 extra frames
  units[INFANTRY].speed = 8;
  units[INFANTRY].infantry = true;
  units[INFANTRY].bullets  = BULLET_SMALL;
  units[INFANTRY].second_shot = true;
  units[INFANTRY].icon = ICON_UNIT_INFANTRY;
  units[INFANTRY].squish=false;
  strcpy(units[INFANTRY].name, "Light Infantry");

  // Unit        : Single Trooper
  // Description : 1 trooper
  units[TROOPER].bmp = (BITMAP *)gfxdata[UNIT_TROOPER].dat;      // pointer to the original 8bit bitmap
  units[TROOPER].bmp_width  = 16*2;
  units[TROOPER].bmp_height = 16*2;
  units[TROOPER].bmp_startpixel = 0;
  units[TROOPER].bmp_frames = 3; // 2 extra frames
  strcpy(units[TROOPER].name, "Trooper");
  units[TROOPER].infantry = true;
  units[TROOPER].bullets = ROCKET_SMALL;
  units[TROOPER].icon      = ICON_UNIT_TROOPER;
  units[TROOPER].squish=false;

  // Unit        : Group Trooper
  // Description : 3 troopers
  units[TROOPERS].bmp = (BITMAP *)gfxdata[UNIT_TROOPERS].dat;      // pointer to the original 8bit bitmap
  units[TROOPERS].bmp_width  = 16*2;
  units[TROOPERS].bmp_height = 16*2;
  units[TROOPERS].bmp_startpixel = 0;
  units[TROOPERS].bmp_frames = 3; // 2 extra frames
  strcpy(units[TROOPERS].name, "Troopers");
  units[TROOPERS].icon      = ICON_UNIT_TROOPERS;
  units[TROOPERS].bullets = ROCKET_SMALL;
  units[TROOPERS].second_shot = true;
  units[TROOPERS].infantry = true;
  units[TROOPERS].squish=false;

    // Unit        : Group Trooper
  // Description : 3 troopers
  units[UNIT_FREMEN].bmp = (BITMAP *)gfxdata[UNIT_TROOPERS].dat;      // pointer to the original 8bit bitmap
  units[UNIT_FREMEN].bmp_width  = 16*2;
  units[UNIT_FREMEN].bmp_height = 16*2;
  units[UNIT_FREMEN].bmp_startpixel = 0;
  units[UNIT_FREMEN].bmp_frames = 3; // 2 extra frames
  strcpy(units[UNIT_FREMEN].name, "Fremen");
  units[UNIT_FREMEN].icon      = ICON_UNIT_FREMEN;
  units[UNIT_FREMEN].bullets = ROCKET_SMALL_FREMEN;
  units[UNIT_FREMEN].second_shot = true;
  units[UNIT_FREMEN].infantry = true;
  units[UNIT_FREMEN].squish=false;

  
  // Unit        : Saboteur
  // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
  units[SABOTEUR].bmp = (BITMAP *)gfxdata[UNIT_SABOTEUR].dat;
  units[SABOTEUR].build_time = 1000;
  units[SABOTEUR].bmp_width  = 16*2;
  units[SABOTEUR].bmp_height = 16*2;
  units[SABOTEUR].bmp_startpixel = 0;
  units[SABOTEUR].bmp_frames = 3; // 2 extra frames
  units[SABOTEUR].speed = 0; // very fast
  units[SABOTEUR].hp = 60;   // quite some health
  units[SABOTEUR].cost = 0;
  units[SABOTEUR].sight = 3; // immense sight! (sorta scouting guys)
  units[SABOTEUR].range = 2;
  units[SABOTEUR].attack_frequency = 0;
  units[SABOTEUR].turnspeed = 0; // very fast
  strcpy(units[SABOTEUR].name, "Saboteur");
  units[SABOTEUR].infantry = true;  
  units[SABOTEUR].icon      = ICON_UNIT_SABOTEUR;
  units[SABOTEUR].squish=false;
  
  // Unit        : Saboteur
  // Description : Special infantry unit, moves like trike, deadly as hell, not detectable on radar!
  units[SANDWORM].speed = 3; // very fast
  units[SANDWORM].bmp = (BITMAP *)gfxdata[UNIT_SANDWORM].dat;
  units[SANDWORM].hp = 9999;
  units[SANDWORM].bmp_width = 24*2;
  units[SANDWORM].bmp_height = 24*2;
  units[SANDWORM].turnspeed = 0; // very fast  
  units[SANDWORM].sight = 16;
  strcpy(units[SANDWORM].name, "Sandworm");
  units[SANDWORM].icon      = ICON_UNIT_SANDWORM;  
  units[SANDWORM].squish=false;


  // Unit        : <name>
  // Description : <description>


 
}


/****************
 Install bullets    
 ****************/
void install_bullets()
{  
  for (int i=0; i < MAX_BULLET_TYPES; i++)
  {
    bullets[i].bmp = NULL; // in case an invalid bitmap; default is a small rocket
    bullets[i].deadbmp = NULL; // in case an invalid bitmap; default is a small rocket
    bullets[i].damage = 0;      // damage to vehicles
    bullets[i].damage_inf = 0;  // damage to infantry
    bullets[i].max_frames = 1;  // 1 frame animation
    bullets[i].max_deadframes = 4; // 4 frame animation
    bullets[i].bmp_width = 8*2;
    bullets[i].sound = -1;    // no sound
  }
  
  // huge rocket/missile
  bullets[ROCKET_BIG].bmp = (BITMAP *)gfxdata[BULLET_ROCKET_LARGE].dat;
  bullets[ROCKET_BIG].deadbmp = EXPLOSION_STRUCTURE01;
  bullets[ROCKET_BIG].bmp_width = 48;
  bullets[ROCKET_BIG].damage = 999;
  bullets[ROCKET_BIG].damage_inf = 999;
  bullets[ROCKET_BIG].max_frames = 0;
  bullets[ROCKET_BIG].sound = SOUND_ROCKET;


    // small rocket (for ornithopter)
  bullets[ROCKET_SMALL_ORNI].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL_ORNI].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL_ORNI].bmp_width = 16;
  bullets[ROCKET_SMALL_ORNI].damage = 12; // they can do pretty damage
  bullets[ROCKET_SMALL_ORNI].damage_inf = 9;
  bullets[ROCKET_SMALL_ORNI].max_frames = 1;
  bullets[ROCKET_SMALL_ORNI].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL_ORNI].max_deadframes = 1;

  // small rocket
  bullets[ROCKET_SMALL].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL].bmp_width = 16;
  bullets[ROCKET_SMALL].damage = 20; // was 8
  bullets[ROCKET_SMALL].damage_inf = 8; // was 4
  bullets[ROCKET_SMALL].max_frames = 1;
  bullets[ROCKET_SMALL].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL].max_deadframes = 1;

  // small rocket - fremen rocket
  bullets[ROCKET_SMALL_FREMEN].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_SMALL].dat;
  bullets[ROCKET_SMALL_FREMEN].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_SMALL_FREMEN].bmp_width = 16;
  bullets[ROCKET_SMALL_FREMEN].damage = 35;
  bullets[ROCKET_SMALL_FREMEN].damage_inf = 20;
  bullets[ROCKET_SMALL_FREMEN].max_frames = 1;
  bullets[ROCKET_SMALL_FREMEN].sound = SOUND_ROCKET_SMALL;
  bullets[ROCKET_SMALL_FREMEN].max_deadframes = 1;

  // normal rocket
  bullets[ROCKET_NORMAL].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[ROCKET_NORMAL].deadbmp = EXPLOSION_ROCKET;
  bullets[ROCKET_NORMAL].bmp_width = 32;
  bullets[ROCKET_NORMAL].damage = 76;
  bullets[ROCKET_NORMAL].damage_inf = 36;  // less damage on infantry
  bullets[ROCKET_NORMAL].max_frames = 1;
  bullets[ROCKET_NORMAL].sound = SOUND_ROCKET;
  bullets[ROCKET_NORMAL].max_deadframes = 4;

  // soldier shot
  bullets[BULLET_SMALL].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_SMALL].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_SMALL].bmp_width = 6;
  bullets[BULLET_SMALL].damage = 4; // vehicles are no match
  bullets[BULLET_SMALL].damage_inf = 10; // infantry vs infantry means big time damage
  bullets[BULLET_SMALL].max_frames = 0;
  bullets[BULLET_SMALL].sound = SOUND_GUN;
  bullets[BULLET_SMALL].max_deadframes = 0;

  // trike shot
  bullets[BULLET_TRIKE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_TRIKE].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TRIKE].bmp_width = 6;
  bullets[BULLET_TRIKE].damage = 3; // trikes do not do much damage to vehicles
  bullets[BULLET_TRIKE].damage_inf = 6; // but more to infantry
  bullets[BULLET_TRIKE].max_frames = 0;
  bullets[BULLET_TRIKE].sound = SOUND_MACHINEGUN;
  bullets[BULLET_TRIKE].max_deadframes = 0;

  // quad shot
  bullets[BULLET_QUAD].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[BULLET_QUAD].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_QUAD].bmp_width = 6;
  bullets[BULLET_QUAD].damage = 6;
  bullets[BULLET_QUAD].damage_inf = 8; // bigger impact on infantry
  bullets[BULLET_QUAD].max_frames = 0;
  bullets[BULLET_QUAD].sound = SOUND_MACHINEGUN;
  bullets[BULLET_QUAD].max_deadframes = 0;

  // normal tank shot
  bullets[BULLET_TANK].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_TANK].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TANK].bmp_width = 8;
  bullets[BULLET_TANK].damage = 12;
  bullets[BULLET_TANK].damage_inf = 4;  // infantry is not much damaged
  bullets[BULLET_TANK].max_frames = 0;
  bullets[BULLET_TANK].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_TANK].max_deadframes = 1;
  
  // siege tank shot
  bullets[BULLET_SIEGE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_SIEGE].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_SIEGE].bmp_width = 8;
  bullets[BULLET_SIEGE].damage = 24;
  bullets[BULLET_SIEGE].damage_inf = 6; // infantry is not as much damaged
  bullets[BULLET_SIEGE].max_frames = 0;
  bullets[BULLET_SIEGE].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_SIEGE].max_deadframes = 2;
    
  // devastator shot 
  bullets[BULLET_DEVASTATOR].bmp     = (BITMAP *)gfxdata[BULLET_DOT_LARGE].dat;
  bullets[BULLET_DEVASTATOR].deadbmp = EXPLOSION_ROCKET_SMALL; // not used anyway
  bullets[BULLET_DEVASTATOR].bmp_width = 8;
  bullets[BULLET_DEVASTATOR].damage = 30;
  bullets[BULLET_DEVASTATOR].damage_inf = 12; // infantry again not much damaged
  bullets[BULLET_DEVASTATOR].max_frames = 0;
  bullets[BULLET_DEVASTATOR].sound = SOUND_EXPL_ROCKET;
  bullets[BULLET_DEVASTATOR].max_deadframes = 1;

  // Gas rocket of a deviator 
  bullets[BULLET_GAS].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[BULLET_GAS].deadbmp = EXPLOSION_GAS;
  bullets[BULLET_GAS].bmp_width = 32;
  bullets[BULLET_GAS].damage = 1;
  bullets[BULLET_GAS].damage_inf = 1;
  bullets[BULLET_GAS].max_frames = 1;
  bullets[BULLET_GAS].max_deadframes = 4;
  bullets[BULLET_GAS].sound = SOUND_ROCKET;

  // normal turret shot
  bullets[BULLET_TURRET].bmp     = (BITMAP *)gfxdata[BULLET_DOT_MEDIUM].dat;
  bullets[BULLET_TURRET].deadbmp = EXPLOSION_BULLET; // not used anyway
  bullets[BULLET_TURRET].bmp_width = 8;
  bullets[BULLET_TURRET].damage = 12;
  bullets[BULLET_TURRET].damage_inf = 12; // infantry is a hard target
  bullets[BULLET_TURRET].max_frames = 0;
  bullets[BULLET_TURRET].max_deadframes = 1; 
  bullets[BULLET_TURRET].sound = SOUND_GUNTURRET;

  // EXEPTION: Shimmer/ Sonic tank
  bullets[BULLET_SHIMMER].bmp     = NULL;
  bullets[BULLET_SHIMMER].deadbmp = -1;
  bullets[BULLET_SHIMMER].bmp_width = 0;
  bullets[BULLET_SHIMMER].damage = 55;
  bullets[BULLET_SHIMMER].damage_inf = 70; // infantry cant stand the sound, die very fast
  bullets[BULLET_SHIMMER].max_frames = 0;
  bullets[BULLET_SHIMMER].max_deadframes = 0;
  bullets[BULLET_SHIMMER].sound = SOUND_SHIMMER;

  // rocket of rocket turret
  bullets[ROCKET_RTURRET].bmp     = (BITMAP *)gfxdata[BULLET_ROCKET_NORMAL].dat;
  bullets[ROCKET_RTURRET].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[ROCKET_RTURRET].bmp_width = 16*2;
  bullets[ROCKET_RTURRET].damage = 25;
  bullets[ROCKET_RTURRET].damage_inf = 10; // infantry is a bit tougher
  bullets[ROCKET_RTURRET].max_frames = 1;
  bullets[ROCKET_RTURRET].sound = SOUND_ROCKET;
  bullets[ROCKET_RTURRET].max_deadframes = 4;

  // SABOTEUR BULLET - FAKE BULLET
  bullets[BULLET_SAB].bmp     = NULL;
  bullets[BULLET_SAB].deadbmp = EXPLOSION_ROCKET_SMALL;
  bullets[BULLET_SAB].bmp_width = 16*2;
  bullets[BULLET_SAB].damage = 9999;
  bullets[BULLET_SAB].damage_inf = 9999; // infantry is a bit tougher
  bullets[BULLET_SAB].max_frames = 1;
  bullets[BULLET_SAB].sound = -1;
  bullets[BULLET_SAB].max_deadframes = 4;


  /****************** EXPLOSIONS *****************/

  bullets[EXPL_ONE].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_ONE].deadbmp = -1;
  bullets[EXPL_ONE].bmp_width = 16*2;
  bullets[EXPL_ONE].damage = 0;
  bullets[EXPL_ONE].max_frames = 0;
  bullets[EXPL_ONE].max_deadframes = 4;

  // 2 of structure explosions
  bullets[EXPL_TWO].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TWO].deadbmp = -1;
  bullets[EXPL_TWO].bmp_width = 16*2;
  bullets[EXPL_TWO].damage = 0;
  bullets[EXPL_TWO].max_frames = 0;
  bullets[EXPL_TWO].max_deadframes = 4;

  // Tank explosion #1
  bullets[EXPL_TANK].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TANK].deadbmp = -1;
  bullets[EXPL_TANK].bmp_width = 24*2;
  bullets[EXPL_TANK].damage = 0;
  bullets[EXPL_TANK].max_frames = 0;
  bullets[EXPL_TANK].max_deadframes = 4;

  // Tank explosion #2
  bullets[EXPL_TANK_TWO].bmp     = (BITMAP *)gfxdata[BULLET_DOT_SMALL].dat;
  bullets[EXPL_TANK_TWO].deadbmp = -1;
  bullets[EXPL_TANK_TWO].bmp_width = 24*2;
  bullets[EXPL_TANK_TWO].damage = 0;
  bullets[EXPL_TANK_TWO].max_frames = 0;
  bullets[EXPL_TANK_TWO].max_deadframes = 4;

}




/*****************************
 Structure Rules
 *****************************/
void install_structures()
{  
  for (int i = 0; i < MAX_STRUCTURETYPES; i++)
  {
    structures[i].bmp = (BITMAP *)gfxdata[BUILD_WINDTRAP].dat; // in case an invalid bitmap, we are a windtrap  
	structures[i].shadow = NULL; // in case an invalid bitmap, we are a windtrap  
    structures[i].sight = 1;
    structures[i].bmp_width = 32*2;
    structures[i].bmp_height = 32*2;    
    structures[i].sight = 5;
    structures[i].hp = 1; // low health
	structures[i].fixhp = -1; // no fixing hp yet
    structures[i].fadecol = -1;
    structures[i].fademax = -1;
    structures[i].power_drain = 0;       // the power that this building drains...
    structures[i].power_give = 0;        // the power that this building gives...
    structures[i].cost = 0;
    structures[i].icon = -1; // stupid default icon
    structures[i].build_time = 0;
    structures[i].list = -1; // no list attached
    strcpy(structures[i].name,   "Unknown");
  } 

  // Single and 4 slabs
  structures[SLAB1].bmp = (BITMAP *)gfxdata[PLACE_SLAB1].dat; // in case an invalid bitmap, we are a windtrap  
  structures[SLAB1].icon = ICON_STR_1SLAB;
  structures[SLAB1].hp   = 25;            // Not functional in-game, only for building
  structures[SLAB1].bmp_width = 16*2;
  structures[SLAB1].bmp_height = 16*2;
  strcpy(structures[SLAB1].name, "Concrete Slab");

  structures[SLAB4].bmp = (BITMAP *)gfxdata[PLACE_SLAB4].dat; // in case an invalid bitmap, we are a windtrap  
  structures[SLAB4].icon = ICON_STR_4SLAB;
  structures[SLAB4].hp   = 75;            // Not functional in-game, only for building
  structures[SLAB4].bmp_width = 32*2;
  structures[SLAB4].bmp_height = 32*2;
  strcpy(structures[SLAB4].name, "4 Concrete Slabs");


  // Concrete Wall
  structures[WALL].bmp = (BITMAP *)gfxdata[PLACE_WALL].dat; // in case an invalid bitmap, we are a windtrap  
  structures[WALL].icon = ICON_STR_WALL;
  structures[WALL].hp   = 75;            // Not functional in-game, only for building
  structures[WALL].bmp_width = 16*2;
  structures[WALL].bmp_height = 16*2;
  strcpy(structures[WALL].name, "Concrete Wall");


  // Structure    : Windtrap
  // Description  : <none>
  structures[WINDTRAP].bmp = (BITMAP *)gfxdata[BUILD_WINDTRAP].dat; 
  structures[WINDTRAP].shadow = (BITMAP *)gfxdata[BUILD_WINDTRAP_SHADOW].dat; // shadow
  structures[WINDTRAP].fadecol = 128;
  structures[WINDTRAP].fademax = 134;
  structures[WINDTRAP].icon  = ICON_STR_WINDTRAP;
  strcpy(structures[WINDTRAP].name, "Windtrap");

  // Structure    : Heavy Factory
  // Description  : <none>
  structures[HEAVYFACTORY].bmp_width = 48*2;
  structures[HEAVYFACTORY].bmp_height = 32*2;    
  structures[HEAVYFACTORY].bmp = (BITMAP *)gfxdata[BUILD_HEAVYFACTORY].dat; 
  structures[HEAVYFACTORY].shadow = (BITMAP *)gfxdata[BUILD_HEAVYFACTORY_SHADOW].dat; // shadow
  structures[HEAVYFACTORY].fadecol = -1;
  structures[HEAVYFACTORY].icon = ICON_STR_HEAVYFACTORY;
  strcpy(structures[HEAVYFACTORY].name, "Heavy Factory");

  // Structure    : Hight Tech Factory (for aircraft only)
  // Description  : <none>
  structures[HIGHTECH].bmp_width = 48*2;
  structures[HIGHTECH].bmp_height = 32*2;    
  structures[HIGHTECH].bmp = (BITMAP *)gfxdata[BUILD_HIGHTECH].dat; 
  structures[HIGHTECH].shadow = (BITMAP *)gfxdata[BUILD_HIGHTECH_SHADOW].dat; 
  structures[HIGHTECH].fadecol = -1;
  structures[HIGHTECH].icon = ICON_STR_HIGHTECH;
  strcpy(structures[HIGHTECH].name, "Hi-Tech");

  // Structure    : Repair
  // Description  : <none>
  structures[REPAIR].bmp_width = 48*2;
  structures[REPAIR].bmp_height = 32*2;    
  structures[REPAIR].bmp = (BITMAP *)gfxdata[BUILD_REPAIR].dat; 
  structures[REPAIR].shadow = (BITMAP *)gfxdata[BUILD_REPAIR_SHADOW].dat; 
  structures[REPAIR].fadecol = -1;
  structures[REPAIR].icon = ICON_STR_REPAIR;
  strcpy(structures[REPAIR].name, "Repair Facility");

  // Structure    : Palace
  // Description  : <none>
  structures[PALACE].bmp_width = 48*2;
  structures[PALACE].bmp_height = 48*2;
  structures[PALACE].bmp = (BITMAP *)gfxdata[BUILD_PALACE].dat;   
  structures[PALACE].shadow = (BITMAP *)gfxdata[BUILD_PALACE_SHADOW].dat;   
  structures[PALACE].icon = ICON_STR_PALACE;
  strcpy(structures[PALACE].name, "Palace");
  
  // Structure    : Light Factory
  // Description  : <none>
  structures[LIGHTFACTORY].bmp_width = 32*2;
  structures[LIGHTFACTORY].bmp_height = 32*2;    
  structures[LIGHTFACTORY].bmp = (BITMAP *)gfxdata[BUILD_LIGHTFACTORY].dat; 
  structures[LIGHTFACTORY].shadow = (BITMAP *)gfxdata[BUILD_LIGHTFACTORY_SHADOW].dat; 
  structures[LIGHTFACTORY].fadecol = -1;
  structures[LIGHTFACTORY].icon = ICON_STR_LIGHTFACTORY;
  strcpy(structures[LIGHTFACTORY].name, "Light Factory");

  // Structure    : Radar
  // Description  : <none>
  structures[RADAR].bmp_width = 32*2;
  structures[RADAR].bmp_height = 32*2;    
  structures[RADAR].bmp = (BITMAP *)gfxdata[BUILD_RADAR].dat; 
  structures[RADAR].shadow = (BITMAP *)gfxdata[BUILD_RADAR_SHADOW].dat; // shadow
  structures[RADAR].sight = 12;
  structures[RADAR].fadecol = -1;
  structures[RADAR].icon = ICON_STR_RADAR;
  strcpy(structures[RADAR].name, "Outpost");

  // Structure    : Barracks
  // Description  : <none>
  structures[BARRACKS].bmp_width = 32*2;
  structures[BARRACKS].bmp_height = 32*2;    
  structures[BARRACKS].bmp = (BITMAP *)gfxdata[BUILD_BARRACKS].dat; 
  structures[BARRACKS].shadow = (BITMAP *)gfxdata[BUILD_BARRACKS_SHADOW].dat; 
  structures[BARRACKS].fadecol = -1;
  structures[BARRACKS].icon = ICON_STR_BARRACKS;
  strcpy(structures[BARRACKS].name, "Barracks");

  // Structure    : WOR
  // Description  : <none>
  structures[WOR].bmp_width = 32*2;
  structures[WOR].bmp_height = 32*2;    
  structures[WOR].bmp = (BITMAP *)gfxdata[BUILD_WOR].dat; 
  structures[WOR].shadow = (BITMAP *)gfxdata[BUILD_WOR_SHADOW].dat; 
  structures[WOR].fadecol = -1;
  structures[WOR].icon = ICON_STR_WOR;
  strcpy(structures[WOR].name, "WOR");



  // Structure    : Silo
  // Description  : <none>
  structures[SILO].bmp_width = 32*2;
  structures[SILO].bmp_height = 32*2;    
  structures[SILO].bmp = (BITMAP *)gfxdata[BUILD_SILO].dat; 
  structures[SILO].shadow = (BITMAP *)gfxdata[BUILD_SILO_SHADOW].dat; 
  structures[SILO].fadecol = -1;
  structures[SILO].icon = ICON_STR_SILO;
  strcpy(structures[SILO].name, "Spice Storage Silo");

  // Structure    : Refinery
  // Description  : <none>
  structures[REFINERY].bmp_width = 48*2;
  structures[REFINERY].bmp_height = 32*2;    
  structures[REFINERY].bmp = (BITMAP *)gfxdata[BUILD_REFINERY].dat; 
  structures[REFINERY].shadow = (BITMAP *)gfxdata[BUILD_REFINERY_SHADOW].dat; 
  structures[REFINERY].fadecol = -1;
  structures[REFINERY].icon = ICON_STR_REFINERY;
  strcpy(structures[REFINERY].name, "Spice Refinery");

  // Structure    : Construction Yard
  // Description  : <none>
  structures[CONSTYARD].bmp_width = 32*2;
  structures[CONSTYARD].bmp_height = 32*2;  
  structures[CONSTYARD].sight = 4;
  structures[CONSTYARD].bmp = (BITMAP *)gfxdata[BUILD_CONSTYARD].dat; 
  structures[CONSTYARD].fadecol = -1;
  structures[CONSTYARD].icon = ICON_STR_CONSTYARD;
  strcpy(structures[CONSTYARD].name, "Construction Yard");

  // Structure    : Starport
  // Description  : You can order units from this structure
  structures[STARPORT].bmp_width = 48*2;
  structures[STARPORT].bmp_height = 48*2;    
  structures[STARPORT].bmp = (BITMAP *)gfxdata[BUILD_STARPORT].dat; 
  structures[STARPORT].shadow = (BITMAP *)gfxdata[BUILD_STARPORT_SHADOW].dat; 
  structures[STARPORT].fadecol = -1;  
  structures[STARPORT].icon  = ICON_STR_STARPORT;
  strcpy(structures[STARPORT].name, "Starport");

  // Structure    : House of IX
  // Description  : Makes it possible for the player to upgrade its Heavy Factory in order to build their special weapon
  structures[IX].bmp_width = 32*2;
  structures[IX].bmp_height = 32*2;    
  structures[IX].bmp = (BITMAP *)gfxdata[BUILD_IX].dat; 
  structures[IX].shadow = (BITMAP *)gfxdata[BUILD_IX_SHADOW].dat; 
  structures[IX].fadecol = -1;  
  structures[IX].icon  = ICON_STR_IX;
  strcpy(structures[IX].name, "House of IX");

  // Structure    : Normal Turret
  // Description  : defence
  structures[TURRET].bmp_width = 16*2;
  structures[TURRET].bmp_height = 16*2;    
  structures[TURRET].bmp = (BITMAP *)gfxdata[BUILD_TURRET].dat;   
  structures[TURRET].shadow = (BITMAP *)gfxdata[BUILD_TURRET_SHADOW].dat;   
  structures[TURRET].fadecol = -1;  
  structures[TURRET].icon  = ICON_STR_TURRET;  
  structures[TURRET].sight = 7;  
  strcpy(structures[TURRET].name, "Gun Turret");

  // Structure    : Rocket Turret
  // Description  : defence
  structures[RTURRET].bmp_width = 16*2;
  structures[RTURRET].bmp_height = 16*2;    
  structures[RTURRET].bmp = (BITMAP *)gfxdata[BUILD_RTURRET].dat;   
  structures[RTURRET].shadow = (BITMAP *)gfxdata[BUILD_RTURRET_SHADOW].dat;   
  structures[RTURRET].fadecol = -1;  
  structures[RTURRET].icon  = ICON_STR_RTURRET;  
  structures[RTURRET].sight = 10;    
  strcpy(structures[RTURRET].name, "Rocket Turret");

  // Structure    : Windtrap
  // Description  : <none>

}


/******************************
 Calculation for SPICE/POWER bars, returns amount of pixels
 ******************************/
 // MAX_W = pixels maxed
 // I = How much we have (CURRENT STATE)
 // W = MAX it can have
float health_bar(float max_w, int i, int w)
{  
  float health = 0.0f;  
  float flHP   = i;
  float flMAX  = w;
  
  if (flHP > flMAX) {
      return max_w;
  }

  // amount of pixels (max_w = 100%)

  health = (float)(flHP / flMAX);

  return (health * max_w);
}


/******************************
 Unit health-bar calculation, returns amount of pictures
 ******************************/
float health_unit(int i, float w)
{  

  float health = 0.0f;    
  float flHP   = unit[i].iHitPoints;
  float flMAX  = units[unit[i].iType].hp;
  
  // amount of pixels (max 16 = 100%)

  health = (float)(flHP / flMAX);

  return (health*w);    
}



/******************************
 Structure health-bar calculation, returns amount of pictures
 ******************************/
float health_structure(int i, int w)
{  
  float health = 0.0f;  
  float flHP   = structure[i]->getHitPoints();
  float flMAX  = structures[structure[i]->getType()].hp;
  
  // amount of pixels (max 16 = 100%)

  health = (float)(flHP / flMAX);

  if (health > 1.0)
      health=1.0;

  return (health*w);
}


// return a border cell, close to iCll
int iFindCloseBorderCell(int iCll)
{
	// Cell x and y coordinates
	int iCllX = iCellGiveX(iCll);
	int iCllY = iCellGiveY(iCll);

	//game.map_height = 64;
	//game.map_width = 64;
	/*
	char msg[255];
	sprintf(msg, "WXH = %dx%d", game.map_width, game.map_height);
	logbook(msg);*/

	// STEP 1: determine starting location of carryall: 
	int iStartCell=-1;
	int lDistance=9999;

	// i will go around the border to find a good startcell:
	// first the horizontal lines
	// then vertical
	
	int tDistance=9999;

	int cll=-1;

	// HORIZONTAL
	for (int iX=0; iX < game.map_width; iX++)
	{
		// check when Y = 0 (top)
		tDistance = ABS_length(iX, 0, iCllX, iCllY);

		if (tDistance < lDistance)
		{
			lDistance = tDistance;
			
			cll = iCellMake(iX, 0);
			
			if (map.occupied(cll) == false)
				iStartCell = cll;

			
		}
		
		// check when Y = map_height (bottom)
		tDistance = ABS_length(iX, game.map_height-1, iCllX, iCllY);

		if (tDistance < lDistance)
		{
			lDistance = tDistance;			

			cll = iCellMake(iX, game.map_height-1);
			
			if (map.occupied(cll) == false)
				iStartCell = cll;			
		}
	}
	
	// VERTICAL
	for (int iY=0; iY < game.map_height; iY++)
	{
		// check when X = 0 (left)
		tDistance = ABS_length(0, iY, iCllX, iCllY);

		if (tDistance < lDistance)
		{
			lDistance = tDistance;			

			cll = iCellMake(0, iY);
			
			if (map.occupied(cll) == false)
				iStartCell = cll;			
		}
		
		// check when XY = map_width (bottom)
		tDistance = ABS_length(game.map_width-1, iY, iCllX, iCllY);

		if (tDistance < lDistance)
		{
			lDistance = tDistance;
			cll = iCellMake(game.map_width-1, iY);
			
			if (map.occupied(cll) == false)
				iStartCell = cll;			
		}
	}

	// return a valid startcell.. :)
	return iStartCell;
}


// Find a primary building for player...
int FIND_PRIMARY_BUILDING(int iType, int iPlayer)
{
	if (DEBUGGING)
	{
	char msg[255];
	sprintf(msg, "Looking for primary building (type %d, name %s, player %d)", iType, structures[iType].name, iPlayer);
	logbook(msg);
	}

	for (int i=0; i < MAX_STRUCTURES; i++)
	{
		// valid , and its for this player too
		if (structure[i] && structure[i]->getOwner() == iPlayer)
		{
			if (structure[i]->getType() == iType)
				if (structure[i]->iFreeAround() > -1) // free!
				{				
					return i; // return this structure
				}
		}
	}

	return -1;
}

int iCellOnScreen(int iCell)
{
    // return 0 when on screen, else the distance..

    if (iCell < 0)
        return 0; // return its on screen, probably some sound that has nothing to do with the battlefield (money, etc)

    int iCellX=iCellGiveX(iCell);
    int iCellY=iCellGiveY(iCell);

    int iMapX = map.scroll_x;
    int iMapY = map.scroll_y;

    int iEndX=iMapX + ((game.screen_x-160)/32); // width of sidebar is 160
    int iEndY=iMapY + ((game.screen_y-42)/32)+1;  // height of upper bar is 42

    if ((iCellX >= iMapX && iCellX <= iEndX) && (iCellY >= iMapY && iCellY <= iEndY))
        return 0; // on screen

    
    int iCalcX = iMapX + (((game.screen_x-160)/32) / 2);
    int iCalcY = iMapY + (((game.screen_y-42)/32)+1)/2;

    // Calc from midst of screen till the cell x,y
    return ABS_length(iCellX, iCellY, iCalcX, iCalcY);
}

// play standard sound
void play_sound_id(int s, int iOnScreen)
{
  if (game.bPlaySound)
  {
    if (gfxaudio[s].dat != NULL && game.iSoundsPlayed < MAXVOICES)
    {
      //play_sample(Data[s].smp, GAME_VOL_SOUND, 128,1000,0);

        /*
        char msg[255];
        sprintf(msg, "Playing sound , iOnScreen=%d", iOnScreen);
        logbook(msg);*/



        // Determine if sound is on screen or not
        if (iOnScreen <= 1)
            game.iSoundsPlayed = play_sample((SAMPLE *)gfxaudio[s].dat, 255, 127,1000,0);
        else
        {            
            int iVol = 255-((255/32) * iOnScreen);
            if (iVol < 0) iVol=0;
            game.iSoundsPlayed = play_sample((SAMPLE *)gfxaudio[s].dat, iVol, 127,1000,0);
        }

    }
  }
}

void play_voice(int iType)
{
//	int iTpe = iType;

	if (player[0].house == HARKONNEN)
		iType++;

	if (player[0].house == ORDOS)
		iType+=2;

	play_sound_id(iType,-1); // pass -1 as 'onscreen' since its a normal sound
}


bool MIDI_music_playing()
{
 if (midi_pos > -1)
   return true;
 else
   return false;
}

void music_volume(int i)
{
    if (game.bMp3)
    {
        if (mp3_music != NULL)
        almp3_adjust_mp3(mp3_music, i, 127, 1000, false);
    }
    else
       set_volume(255, i);

    
}


void mp3_play_file(char filename[255])
{

	
  char *data;  // mp3 file in memory
  int len;     // length of file(also the buffer)
  FILE *f = NULL;  // file.mp3

  len =   (int)file_size(filename);

  //allegro_message("Len = %d", len);
  
  data = new char[len];
  f = fopen(filename, "r");

  if (f != NULL)
  {
  fread(data, 1, len, f); 
  fclose(f);
  }
  else
  {
       logbook("MP3: Could not find mp3 file for add-on, switching to MIDI mode");
       allegro_message("Could not find MP3 file, add-on incomplete. Switching to MIDI mode");
       game.bMp3=false;

       if (mp3_music != NULL)
        almp3_destroy_mp3(mp3_music);

       mp3_music=NULL;
     
       return;

  }
  
  almp3_destroy_mp3(mp3_music); // stop music

  mp3_music = almp3_create_mp3(data, len); 
    
  // play music, use big buffer
  almp3_play_mp3(mp3_music, 32768, 255, 128);

  music_volume(game.iMusicVolume);
}




// play type of music
void play_music(int iType)
{
    // Types of music:
    /*
#define MUSIC_WIN           0
#define MUSIC_LOSE          1
#define MUSIC_ATTACK        2
#define MUSIC_PEACE         3
#define MUSIC_MENU          4
#define MUSIC_CONQUEST      5
#define MUSIC_BRIEFING      6*/

    // WIN
    // LOSE
    // ATTACK
    // PEACE
    // MENU
    // SELECT CONQUEST
    
    game.iMusicType = iType;

    int iNumber=0;
    
// win and lose has 3 music files
    if (iType == 0 || iType == 1)
        iNumber=rnd(3)+1;
    
// attack has 6 files
    if (iType == 2)
        iNumber=rnd(6)+1;

// peace has 9 files
    if (iType == 3)
        iNumber=rnd(9)+1;

    if (iType == 4)
        iNumber=MIDI_MENU;

    if (iType == 5)
        iNumber=MIDI_SCENARIO;

    if (iType == 6)
    {
        if (game.iHouse == ATREIDES)     iNumber=MIDI_MENTAT_ATR;
        if (game.iHouse == HARKONNEN)     iNumber=MIDI_MENTAT_HAR;
        if (game.iHouse == ORDOS)     iNumber=MIDI_MENTAT_ORD;
      
    }

    

    // In the end, when mp3, play it:
    if (game.bMp3)
    {
        char filename[50];
        memset(filename, 0, sizeof(filename));

        if (iType == 0)
            sprintf(filename, "mp3/win%d.mp3", iNumber);

        if (iType == 1)
            sprintf(filename, "mp3/lose%d.mp3", iNumber);

        if (iType == 2)
            sprintf(filename, "mp3/attack%d.mp3", iNumber);

        if (iType == 3)
            sprintf(filename, "mp3/peace%d.mp3", iNumber);

		if (iType == 4)
            sprintf(filename, "mp3/menu.mp3");

        if (iType == 5)
            sprintf(filename, "mp3/nextconq.mp3");

        if (iType == 6)
        {   
            if (game.iHouse == ATREIDES)
                sprintf(filename, "mp3/mentata.mp3");
            if (game.iHouse == HARKONNEN)
                sprintf(filename, "mp3/mentath.mp3");
            if (game.iHouse == ORDOS)
                sprintf(filename, "mp3/mentato.mp3");
        }

        mp3_play_file(filename);
    }
    else
    {
        iNumber--; // correct for most midis
        
        if (iType == 0)
            iNumber = MIDI_WIN01+(iNumber);

        if (iType == 1)
            iNumber = MIDI_LOSE01+(iNumber);

        if (iType == 2)
            iNumber = MIDI_ATTACK01+(iNumber);

        if (iType == 3)
            iNumber = MIDI_BUILDING01+(iNumber);

        // single ones are 'corrected back'... 
        if (iType == 4)
            iNumber = iNumber+1;

        if (iType == 5)
            iNumber = iNumber+1;

        if (iType == 6)
            iNumber = iNumber+1;


        // play midi file
        play_midi((MIDI *)gfxaudio[iNumber].dat, 0);

    } 

    

}

/******************************
 Bullet creation
 ******************************/
int create_bullet(int type, int cell, int goal_cell, int ownerunit, int ownerstruc)
{
  int new_id=-1;

  for (int i=0; i < MAX_BULLETS; i++)
    if (bullet[i].bAlive==false)
    {
      new_id = i;
      break;
    }

  if (new_id < 0)
    return -1;  // failed
  
  if (type < 0)
    return -1; // failed

  
  bullet[new_id].init();

  bullet[new_id].iType = type;
  bullet[new_id].iCell = cell;
  bullet[new_id].iOwnerStructure = ownerstruc;
  bullet[new_id].iOwnerUnit = ownerunit;

  bullet[new_id].iGoalCell = goal_cell;
    
  bullet[new_id].bAlive=true;
  bullet[new_id].iFrame = 0;

  bullet[new_id].iOffsetX = 8 + rnd(9);
  bullet[new_id].iOffsetY = 8 + rnd(9);
  
  
  if (ownerunit > -1)
  {
      bullet[new_id].iPlayer = unit[ownerunit].iPlayer;
      // create spot
      map.clear_spot(cell, 3, unit[ownerunit].iPlayer);

  }

  if (ownerstruc > -1)
  {
      bullet[new_id].iPlayer = structure[ownerstruc]->getOwner();
      map.clear_spot(cell, 3, structure[ownerstruc]->getOwner());
  }


  // play sound (when we have one)
  if (bullets[type].sound > -1)
      play_sound_id(bullets[type].sound, iCellOnScreen(cell));

  return new_id;
}


// Make shimmer
void Shimmer(int r, int x, int y)
{
  // r = radius
  // X, Y = position

  /* 
    Logic

    Each X and Y position will be taken and will be switched with a randomly choosen neighbouring
    pixel.

    Shimmer effect is done on BMP_SCREEN only.
  */

  int x1,y1;
  int nx,ny;
  int gp=0;     // Get Pixel Result
  int tc=0;

  // go around 360 fDegrees (twice as fast now)
  for (int dr=0; dr < r; dr+=3)
  {
    for (double d=0; d < 360; d++)
    {
       x1 = (x + (cos(d)*(dr)));
       y1 = (y + (sin(d)*(dr)));
  
       if (x1 < 0) x1=0;
       if (y1 < 0) y1=0;
       if (x1 >= game.screen_x) x1 = game.screen_x-1;
       if (y1 >= game.screen_y) y1 = game.screen_y-1;

       gp = getpixel(bmp_screen, x1,y1); // use this inline function to speed up things.
       // Now choose random spot to 'switch' with.
       nx=(x1-1) + rnd(2);
       ny=(y1-1) + rnd(2);

       if (nx < 0) nx=0;
       if (ny < 0) ny=0;
       if (nx >= game.screen_x) nx=game.screen_x-1;
       if (ny >= game.screen_y) ny=game.screen_y-1;

       tc = getpixel(bmp_screen, nx,ny);

       if (gp > -1 && tc > -1)
       {
        // Now switch colors
        putpixel(bmp_screen, nx, ny, gp);
        putpixel(bmp_screen, x1, y1, tc);
       }

    }
  } 

}


void LOAD_SCENE(char file[30])
{
  gfxmovie = NULL;

  char filename[128];
  sprintf(filename, "data/scenes/%s.dat", file);

  gfxmovie = load_datafile(filename);

  char msg[255];
  sprintf(msg, "LOAD SCENE: %s", filename);
  logbook(msg);

  if (gfxmovie != NULL)  
  {    
    game.iMovieFrame=0;
    
  }
  else
  {   
    gfxmovie=NULL;
    game.iMovieFrame=-1;
    logbook("FAILED");
  }

}

// Skirmish map initialization
void INIT_PREVIEWS()
{
    for (int i=0; i < MAX_SKIRMISHMAPS; i++)
    {	
        PreviewMap[i].terrain = NULL;

		// clear out name
        memset(PreviewMap[i].name , 0, sizeof(PreviewMap[i].name));

        // clear out map data
        memset(PreviewMap[i].mapdata , -1, sizeof(PreviewMap[i].mapdata));
		
        //sprintf(PreviewMap[i].name, "Map %d", i);
		// 
        PreviewMap[i].iPlayers=0;

        PreviewMap[i].iStartCell[0]=-1;
        PreviewMap[i].iStartCell[1]=-1;
        PreviewMap[i].iStartCell[2]=-1;
        PreviewMap[i].iStartCell[3]=-1;
        PreviewMap[i].iStartCell[4]=-1;
    }

	sprintf(PreviewMap[0].name, "RANDOM MAP");
	//PreviewMap[0].terrain = (BITMAP *)gfxinter[BMP_UNKNOWNMAP].dat;	
	PreviewMap[0].terrain = create_bitmap(128,128);
}

// 8 bit memory putpixel
void memory_putpixel(BITMAP *bmp, int x, int y, int color)
{
	bmp->line[y][x] = color;
}

void lit_windtrap_color(BITMAP *bmp, int iColor)
{
	for (int x=0; x < bmp->w; x++)
	{
		for (int y=0; y < bmp->h; y++)
		{
            // masked
            if (getpixel(bmp, x, y) == makecol(40,40,182))            
                _putpixel16(bmp, x, y, iColor);            
		}
	}
}

void mask_to_color(BITMAP *bmp, int color)
{
	//BITMAP *temp16;
	//temp16=create_bitmap(bmp->w, bmp->h);

	//clear_to_color(temp16, makecol(255,0,255));

	//masked_blit(bmp, temp16, 0, 0, 0, 0, bmp->w, bmp->h);
	
	for (int x=0; x < bmp->w; x++)
	{
		for (int y=0; y < bmp->h; y++)
		{
			int c = getpixel(bmp, x, y);

			if (c != makecol(0,0,0))
			{
				// masked
				putpixel(bmp, x, y, makecol(255,255,255));
				//allegro_message("Non pink detected");
			}			
		}
	}

	//draw_sprite(bmp, temp16, 0, 0);
	//masked_blit(temp16, bmp, 0, 0, 0, 0, bmp->w, bmp->h);
	//destroy_bitmap(temp16);
	
}

