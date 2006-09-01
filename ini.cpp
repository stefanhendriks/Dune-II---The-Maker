/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"
#include "d2tmc.h" 

#include "IniReader.h"

IniReader::IniReader() {
    
    // set key / value for sections
    sectionMap.setValue("UNITS", INI_UNITS);
    sectionMap.setValue("SKIRMISH", INI_SKIRMISH);
    sectionMap.setValue("STRUCTURES", INI_STRUCTURES);
    sectionMap.setValue("REINFORCEMENTS", INI_REINFORCEMENTS);
    sectionMap.setValue("MAP", INI_MAP);
    sectionMap.setValue("BASIC", INI_BASIC);
    sectionMap.setValue("Atreides", INI_HOUSEATREIDES);
    sectionMap.setValue("Ordos", INI_HOUSEORDOS);
    sectionMap.setValue("Harkonnen", INI_HOUSEHARKONNEN);
    sectionMap.setValue("Sardaukar", INI_HOUSESARDAUKAR);
    sectionMap.setValue("Fremen", INI_HOUSEFREMEN);
    sectionMap.setValue("Mercenary", INI_HOUSEMERCENARY);
    // set key / value for ..

}


void IniReader::ReadLine(FILE *file) {

  if (file == NULL) {
      return;
  }
  char ch;
  char result[MAX_LINE_LENGTH];
  int pos=0;

  memset(result, 0, sizeof(result));

  while ((feof(file) == 0) && ((ch=fgetc(file)) != '\n'))
  {
    if (pos < MAX_LINE_LENGTH) {
        result[pos]=ch;
        pos++;
        putchar(ch);
    } else {
        break;
    }
 }

  memcpy(currentLine, result, MAX_LINE_LENGTH);
}

// Reads out an entire sentence and returns it
void INI_Sentence(FILE *f, char result[MAX_LINE_LENGTH])
{ 
  char ch;
  int pos=0;

  // clear out entire string
 for (int i=0; i < MAX_LINE_LENGTH;i++)
   result[i] = '\0';
  
 while ((feof(f) == 0) && ((ch=fgetc(f)) != '\n'))
 {
  result[pos]=ch;
  pos++;  
  
  // do not allow strings greater then 80 characters. This check prevents a crash for
  // users who do exceed the limit.
  if (pos> (MAX_LINE_LENGTH-1))
    break;

  putchar(ch);   
 }


}

int IniReader::getSection() {
    int result = sectionMap.getValue(key);
    
    if (result > -1 && result != currentSection) {
        return result;
    } 
    
    return currentSection;
}

/*********************************************************************************/
// Reads out INPUT , will check for a [ at [0] and then checks till ], it will fill section[]
// with the chars in between. So : [MAP] -> section = 'MAP'. Use function INI_SectionType(..)
// to get the correct ID for that.
void INI_Section(char input[MAX_LINE_LENGTH], char section[30])
{
 
  int pos=0;
  int end_pos=-1;  

  // clear out entire string
 for (int i=0; i < 30;i++)
   section[i] = '\0';
 
 // check if the first character is a '['
 if (input[0] == '[')
 {
   pos=1; // Begin at character 1
   
   while (pos < (MAX_LINE_LENGTH-1))
   {
    if (input[pos] == ']')
    {
      end_pos=pos-1;
      break;
    }
    pos++;    
   }
 
 if (end_pos > 1 && end_pos < 29)
 {
   for (int wc=0; wc < end_pos; wc++)
     section[wc]=input[wc+1];

   section[end_pos]='\0'; // terminate string   
 }
 }

}


// Reads out INPUT and will check for an '=' Everything at the left of the
// '=' IS a word and will be put in 'word[]'. Use function INI_WordType(char word[25]) to get
// the correct ID tag.
void INI_Word(char input[MAX_LINE_LENGTH], char word[25])
{
 int pos=0;
 int word_pos=-1;

  // clear out entire string
 for (int i=0; i < 25;i++)
   word[i] = '\0';
 
 
 while (pos < (MAX_LINE_LENGTH-1))
 {
  if (input[pos] == '=')
  {
    word_pos=pos;
    break;
  }
    pos++;  
 }
 
 if (word_pos > -1 && word_pos < 23)
 {
   for (int wc=0; wc < word_pos; wc++)
     word[wc]=input[wc];

   word[word_pos]='\0'; // terminate string   
 }
}
// Reads out word[], checks structure type, and returns actual source-id
int INI_StructureType(char word[45])
{

  if (strcmp(word, "WINDTRAP") == 0)
    return WINDTRAP;

  if (strcmp(word, "PALACE") == 0)
    return PALACE;

  if (strcmp(word, "HEAVYFACTORY") == 0)
    return HEAVYFACTORY;

  if (strcmp(word, "LIGHTFACTORY") == 0)
    return LIGHTFACTORY;

  if (strcmp(word, "CONSTYARD") == 0)
    return CONSTYARD;

  if (strcmp(word, "SILO") == 0)
    return SILO;

  if (strcmp(word, "HIGHTECH") == 0)
    return HIGHTECH;
  
  if (strcmp(word, "IX") == 0)
    return IX;
  
  if (strcmp(word, "REPAIR") == 0)
    return REPAIR;

  if (strcmp(word, "RADAR") == 0)
    return RADAR;

  if (strcmp(word, "REFINERY") == 0)
    return REFINERY;

  if (strcmp(word, "WOR") == 0)
    return WOR;

  if (strcmp(word, "BARRACKS") == 0)
    return BARRACKS;

  if (strcmp(word, "STARPORT") == 0)
    return STARPORT;

  if (strcmp(word, "TURRET") == 0)
    return TURRET;

  if (strcmp(word, "ROCKETTURRET") == 0)
    return RTURRET;

  // - not really structures -
  if (strcmp(word, "SLAB") == 0)
    return SLAB1;

  if (strcmp(word, "4SLAB") == 0)
    return SLAB4;

  if (strcmp(word, "WALL") == 0)
    return WALL;


  // notify user here
  alert("ERROR: Could not identify structure type", word, "", "OK", NULL, 13, 0);  
  
  
   return -1;

}

// return house relation ship
int INI_HouseType(char word[35])
{
  if (strcmp(word, "Atreides") == 0)
    return ATREIDES;

  if (strcmp(word, "Harkonnen") == 0)
    return HARKONNEN;
  
  if (strcmp(word, "Ordos") == 0)
    return ORDOS;

  if (strcmp(word, "Fremen") == 0)
    return FREMEN;

  if (strcmp(word, "Mercenary") == 0)
    return MERCENARY;

  if (strcmp(word, "Sardaukar") == 0)
    return SARDAUKAR;

  // d2tm specifics:
  //if (strcmp(word, "Guild") == 0)
    //return HOUSE_MERCENARY;

  return -1;
}

// Reads out word[], checks unit type, and returns actual source-id
int INI_UnitType(char word[35])
{
  
  if (strcmp(word, "HARVESTER") == 0)
    return HARVESTER;

  if (strcmp(word, "DEVIATOR") == 0)
    return DEVIATOR;
  
  if (strcmp(word, "TRIKE") == 0)
    return TRIKE;

  if (strcmp(word, "QUAD") == 0)
    return QUAD;

  if (strcmp(word, "MCV") == 0)
    return MCV;

  if (strcmp(word, "RAIDER") == 0)
    return RAIDER;

  if (strcmp(word, "SOLDIER") == 0)
    return SOLDIER;

  if (strcmp(word, "INFANTRY") == 0)
    return INFANTRY;

  if (strcmp(word, "COMBATTANK") == 0)
    return TANK;

  if (strcmp(word, "SONICTANK") == 0)
    return SONICTANK;

  if (strcmp(word, "SIEGETANK") == 0)
    return SIEGETANK;

  if (strcmp(word, "DEVASTATOR") == 0)
    return DEVASTATOR;

  if (strcmp(word, "CARRYALL") == 0)
    return CARRYALL;
  
  if (strcmp(word, "ORNITHOPTER") == 0)
    return ORNITHOPTER;
  
  if (strcmp(word, "LAUNCHER") == 0)
    return LAUNCHER;

  if (strcmp(word, "TROOPER") == 0)
    return TROOPER;

  if (strcmp(word, "TROOPERS") == 0)
    return TROOPERS;

  if (strcmp(word, "FREMEN") == 0)
    return UNIT_FREMEN;

  if (strcmp(word, "SABOTEUR") == 0)
    return SABOTEUR;

  if (strcmp(word, "MISSILE") == 0)
    return   UPGR_MISSILE;



  alert("ERROR: Could not identify unit type", word, "", "OK", NULL, 13, 0);  
  
  
   return -1;
}


// Reads out word[], does a string compare and returns type id
int INI_WordType(char word[25], int section)
{
	if (section == SEC_REGION)
	{   
		if (strcmp(word, "Region") == 0)
			return WORD_REGION;

        if (strcmp(word, "RegionConquer") == 0)
			return WORD_REGIONCONQUER;

		if (strcmp(word, "House") == 0)
			return WORD_REGIONHOUSE;

		if (strcmp(word, "Text") == 0)
			return WORD_REGIONTEXT;

		if (strcmp(word, "Select") == 0)
			return WORD_REGIONSELECT;

		return WORD_NONE;
	}

    if (section == INI_SCEN || 
        section == INI_DESCRIPTION ||
        section == INI_BRIEFING ||
        section == INI_ADVICE ||
        section == INI_LOSE ||
        section == INI_WIN )
    {
		if (strcmp(word, "Number") == 0)
			return WORD_NUMBER;

   		if (strcmp(word, "Text") == 0)
			return WORD_REGIONTEXT;

        return WORD_NONE;
    }

  if (section == INI_SKIRMISH)
  {
    if (strcmp(word, "Title") == 0)
        return WORD_MAPNAME;

	
	if (strcmp(word, "StartCell") == 0)
        return WORD_STARTCELL;

  }

  if (section == INI_GAME)
  {
    if (strcmp(word, "ModId") == 0)
      return WORD_MODID;
  }
  else if (section == INI_BASIC)
  {
	if (strcmp(word, "CursorPos") == 0)
		return WORD_FOCUS;

    if (strcmp(word, "BriefPicture") == 0)
		return WORD_BRIEFPICTURE;


  }
  else if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY)
  { 
	if (strcmp(word, "Team") == 0)
		return WORD_TEAM;

	if (strcmp(word, "Credits") == 0)
		return WORD_CREDITS;

    if (strcmp(word, "Quota") == 0)
		return WORD_QUOTA;

	if (strcmp(word, "Brain") == 0)
		return WORD_BRAIN;

	//if (strcmp(word, "Skill") == 0)
		//return WORD_SKILL;

	if (strcmp(word, "House") == 0)
		return WORD_HOUSE;

	if (strcmp(word, "Focus") == 0)
		return WORD_FOCUS;
  }
  /*
  else if (section == INI_MENU)
  {
    if (strcmp(word, "TitleBitmap") == 0)
      return WORD_TITLEBITMAP;

    if (strcmp(word, "ClickSound") == 0)
      return WORD_MENUCLICKSOUND;
  }
  else if (section == INI_MOD)
  {
    // Title of mod
    if (strcmp(word, "Title") == 0)
      return WORD_TITLE;

    // Version of mod
    if (strcmp(word, "Version") == 0)
      return WORD_VERSION;

    // Dir of mod
    if (strcmp(word, "Dir") == 0)
      return WORD_DIR;
  
  }
  else if (section == INI_SIDEBAR)
  {
    if (strcmp(word, "ColorRed") == 0)
      return WORD_RED;

    if (strcmp(word, "ColorBlue") == 0)
      return WORD_BLUE;

    if (strcmp(word, "ColorGreen") == 0)
      return WORD_GREEN;
  }
  else if (section == INI_MOUSE)
  {
    // normal mouse
    if (strcmp(word, "Normal") == 0)
      return WORD_MOUSENORMAL;
    
    // cannot move
    if (strcmp(word, "CannotMove") == 0)
      return WORD_MOUSENOMOVE;
    
    // Move
    if (strcmp(word, "Move") == 0)
      return WORD_MOUSEMOVE;

    // Attack
    if (strcmp(word, "Attack") == 0)
      return WORD_MOUSEATTACK;

    // Deploy
    if (strcmp(word, "Deploy") == 0)
      return WORD_MOUSEDEPLOY;
  }*/
  else if (section == INI_MAP)
  {
    // When reading [MAP], interpet the 'width' and 'height' for default width and height
    // for the Map Editor

    // Map width
    if (strcmp(word, "Width") == 0)
      return WORD_MAPWIDTH;

    // Map heigth
    if (strcmp(word, "Height") == 0)
      return WORD_MAPHEIGHT;

	// Dune II scens have a seed
	if (strcmp(word, "Seed") == 0)
      return WORD_MAPSEED;

    // Dune II scens have spice blooms
	if (strcmp(word, "Bloom") == 0)
      return WORD_MAPBLOOM;

	// Dune II scens have spice blooms
	if (strcmp(word, "Field") == 0)
      return WORD_MAPFIELD;

    
  }  
  else if (section == INI_BULLETS)
  {
    // Bitmap
    if (strcmp(word, "Bitmap") == 0)
      return WORD_BITMAP;

    // Dead Animation Bitmap
    if (strcmp(word, "BitmapExplosion") == 0)
      return WORD_BITMAP_DEAD;

    // Bitmap width
    if (strcmp(word, "BitmapWidth") == 0)
      return WORD_BITMAP_WIDTH;

    // Frames
    if (strcmp(word, "BitmapFrames") == 0)
      return WORD_BITMAP_FRAMES;

    // Dead frames
    if (strcmp(word, "BitmapExplFrames") == 0)
      return WORD_BITMAP_DEADFRAMES;

    // Damage
    if (strcmp(word, "Damage") == 0)
      return WORD_DAMAGE;
  
    // Definition
    if (strcmp(word, "Definition") == 0)
      return WORD_DEFINITION;

    if (strcmp(word, "Sound") == 0)
      return WORD_SOUND;

  }
  else if (section == INI_STRUCTURES)
  {
    // HitPoints
    if (strcmp(word, "HitPoints") == 0)
      return WORD_HITPOINTS;

	// 'getting fixed 'HitPoints
    if (strcmp(word, "FixPoints") == 0)
      return WORD_FIXHP;

    // Power drain
    if (strcmp(word, "PowerDrain") == 0)
      return WORD_POWERDRAIN;

    // Power give
    if (strcmp(word, "PowerGive") == 0)
      return WORD_POWERGIVE;

    // Cost
    if (strcmp(word, "Cost") == 0)
      return WORD_COST;

    // Build time
    if (strcmp(word, "BuildTime") == 0)
      return WORD_BUILDTIME;

  }
  else if (section == INI_UNITS)
  {
    // Bitmap
    if (strcmp(word, "Bitmap") == 0)
      return WORD_BITMAP;

    // TOP Bitmap
    if (strcmp(word, "BitmapTop") == 0)
      return WORD_BITMAP_TOP;

    // ICON Bitmap
    if (strcmp(word, "Icon") == 0)
      return WORD_ICON;

    // WIDTH and HEIGHT of a bitmap
    if (strcmp(word, "BitmapWidth") == 0)
      return WORD_BITMAP_WIDTH;

    // Bitmap Height
    if (strcmp(word, "BitmapHeight") == 0)
      return WORD_BITMAP_HEIGHT;

    // HitPoints
    if (strcmp(word, "HitPoints") == 0)
      return WORD_HITPOINTS;

    // Cost
    if (strcmp(word, "Cost") == 0)
      return WORD_COST;

    // Bullet Type
    if (strcmp(word, "BulletType") == 0)
      return WORD_BULLETTYPE;

    // Move speed
    if (strcmp(word, "MoveSpeed") == 0)
      return WORD_MOVESPEED;

    // Turn speed
    if (strcmp(word, "TurnSpeed") == 0)
      return WORD_TURNSPEED;

    // Attack frequency
    if (strcmp(word, "AttackFrequency") == 0)
      return WORD_ATTACKFREQ;

    // Sight
    if (strcmp(word, "Sight") == 0)
      return WORD_SIGHT;
    
    // Range
    if (strcmp(word, "Range") == 0)
      return WORD_RANGE;
      
    // Build time
    if (strcmp(word, "BuildTime") == 0)
      return WORD_BUILDTIME;

    // Description
    if (strcmp(word, "Description") == 0)
      return WORD_DESCRIPTION;

    // BOOLEANS
    if (strcmp(word, "IsHarvester") == 0)
      return WORD_ISHARVESTER;

    if (strcmp(word, "SecondShot") == 0)
      return WORD_SECONDSHOT;

    if (strcmp(word, "IsInfantry") == 0)
      return WORD_ISINFANTRY;

    if (strcmp(word, "IsAirborn") == 0)
      return WORD_ISAIRBORN;

    if (strcmp(word, "AbleToCarry") == 0)
      return WORD_ABLETOCARRY;

    if (strcmp(word, "FreeRoam") == 0)
      return WORD_FREEROAM;

    // HARVESTER 
    if (strcmp(word, "MaxCredits") == 0)
      return WORD_HARVESTLIMIT;

    if (strcmp(word, "HarvestSpeed") == 0)
      return WORD_HARVESTSPEED;

    if (strcmp(word, "HarvestAmount") == 0)
      return WORD_HARVESTAMOUNT;

  }
  else if (section == INI_TEAMS)
  {
    // SwapColor
    if (strcmp(word, "SwapColor") == 0)
      return WORD_SWAPCOLOR;

    // Red MiniMap value
    if (strcmp(word, "MapColorRed") == 0)
      return WORD_HOUSE_RED;

    // Green MiniMap value
    if (strcmp(word, "MapColorGreen") == 0)
      return WORD_HOUSE_GREEN;

    // Blue MiniMap value
    if (strcmp(word, "MapColorBlue") == 0)
      return WORD_HOUSE_BLUE;

  }
  /*
  else if (section == INI_ICONS)
  {
    // Process all Icons here
    if (strlen(word) > 1)
      return WORD_ICONID;
    else
      return WORD_NONE;
  }
  else if (section == INI_BITMAPS)
  {
    // Process all Bitmaps here
    if (strlen(word) > 1)
      return WORD_BITMAPID;
    else
      return WORD_NONE;
  }*/
  else if (section == INI_STRUCTURES)
  {
    if (strlen(word) > 1)
    {      
      // Structure properties
      if (strcmp(word, "PreBuild") == 0)
        return WORD_PREBUILD;
      
      if (strcmp(word, "Description") == 0)
        return WORD_DESCRIPTION;
     
      if (strcmp(word, "Power") == 0)
        return WORD_POWER;        // What power it takes
     
    }
    else 
      return WORD_NONE;   
  }
  else if (section == INI_HOUSES)
  {
	  // each house has properties..

	  if (strcmp(word, "ColorR") == 0)
        return WORD_RED;

	  if (strcmp(word, "ColorG") == 0)
        return WORD_GREEN;

	  if (strcmp(word, "ColorB") == 0)
        return WORD_BLUE;

	  // and specific stuff:
	  if (strcmp(word, "FirePower") == 0)
        return WORD_FIREPOWER;

	  if (strcmp(word, "FireRate") == 0)
        return WORD_FIRERATE;

	  if (strcmp(word, "StructPrice") == 0)
        return WORD_STRUCTPRICE;

	  if (strcmp(word, "UnitPrice") == 0)
        return WORD_UNITPRICE;

	  if (strcmp(word, "Speed") == 0)
        return WORD_SPEED;

	  if (strcmp(word, "BuildSpeed") == 0)
        return WORD_BUILDSPEED;

	  if (strcmp(word, "HarvestSpeed") == 0)
        return WORD_HARVESTSPEED;
		
		if (strcmp(word, "DumpSpeed") == 0)
        return WORD_DUMPSPEED;

  }
  


  return WORD_NONE;
}


// Scenario section types
int SCEN_INI_SectionType(char section[30], int last)
{
// if (strcmp(section, "PLAYERS") == 0)
  //  return INI_PLAYER;
 
 //if (strcmp(section, "MAP") == 0)
   //return INI_TERRAIN;

 if (strcmp(section, "UNITS") == 0)
    return INI_UNITS;

 if (strcmp(section, "SKIRMISH") == 0)
    return INI_SKIRMISH;

 if (strcmp(section, "STRUCTURES") == 0)
    return INI_STRUCTURES;

 if (strcmp(section, "REINFORCEMENTS") == 0)
    return INI_REINFORCEMENTS;

 if (strcmp(section, "MAP") == 0)
    return INI_MAP;

 if (strcmp(section, "BASIC") == 0)
    return INI_BASIC;

 if (strcmp(section, "Atreides") == 0)
    return INI_HOUSEATREIDES;

 if (strcmp(section, "Ordos") == 0)
    return INI_HOUSEORDOS;

 if (strcmp(section, "Harkonnen") == 0)
    return INI_HOUSEHARKONNEN;

 if (strcmp(section, "Sardaukar") == 0)
    return INI_HOUSESARDAUKAR;

 if (strcmp(section, "Fremen") == 0)
    return INI_HOUSEFREMEN;

 if (strcmp(section, "Mercenary") == 0)
    return INI_HOUSEMERCENARY;



  // When nothing found; we assume its just a new ID tag for some unit or structure
  // Therefor we return the last known SECTION ID so we can assign the proper WORD ID's  
  return last;
}


// GAME Section Types
int GAME_INI_SectionType(char section[30], int last)
{

// if (strcmp(section, "BULLETS") == 0)
//   return INI_BULLETS; 
 
 if (strcmp(section, "UNITS") == 0)
   return INI_UNITS; 
 
 if (strcmp(section, "STRUCTURES") == 0)
   return INI_STRUCTURES; 
 
  // When nothing found; we assume its just a new ID tag for some unit or structure
  // Therefor we return the last known SECTION ID so we can assign the proper WORD ID's  
  return last;
}
// Reads out section[], does a string compare and returns type id
int INI_SectionType(char section[30], int last)
{  
  if (strcmp(section, "MAP") == 0)
    return INI_MAP;

  if (strcmp(section, "SKIRMISH") == 0)
    return INI_SKIRMISH;

   if (strcmp(section, "DESCRIPTION") == 0)
    return INI_DESCRIPTION;

  if (strcmp(section, "SCEN") == 0)
    return INI_SCEN;

  if (strcmp(section, "BRIEFING") == 0)
    return INI_BRIEFING;

  if (strcmp(section, "WIN") == 0)
    return INI_WIN;

  if (strcmp(section, "LOSE") == 0)
    return INI_LOSE;

  if (strcmp(section, "ADVICE") == 0)
    return INI_ADVICE;

  if (strcmp(section, "HOUSES") == 0)
    return INI_HOUSES;

  if (strcmp(section, "UNITS") == 0)
    return INI_UNITS;

  if (strcmp(section, "STRUCTURES") == 0)
  {
    alert("Structure Section found", section, "", "OK", NULL, 13, 0);    
    return INI_STRUCTURES;
  }
 
  alert("No SECTION id found, assuming its an ID nested in section", section, "", "OK", NULL, 13, 0);  

  // When nothing found; we assume its just a new ID tag for some unit or structure
  // Therefor we return the last known SECTION ID so we can assign the proper WORD ID's  
  return last;
}

// Reads out 'result' and will return the value after the '='. Returns integer.
// For CHAR returns see "INI_WordValueCHAR(char result[80]);
int INI_WordValueINT(char result[MAX_LINE_LENGTH])
{
 int pos=0;
 int is_pos=-1;
 
 while (pos < (MAX_LINE_LENGTH-1))
 {
  if (result[pos] == '=')
  {
    is_pos=pos;
    break;
  }
    pos++;
 }
 
 if (is_pos > -1)
 {
   // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
   // space.
   int end_pos=-1;

  while (pos < (MAX_LINE_LENGTH-1))
  {
    if (result[pos] == '\0')
    {
      end_pos=pos;
      break;
    }
      pos++;
  } 
  
  // End position found!
  if (end_pos > -1)
  {
    // We know the END position. We will use that piece of string to read out a number.
    char number[10];
  
    // clear out entire string
    for (int i=0; i < 10;i++)    
      number[i] = '\0';

    // Copy the part to 'number', Make sure we won't get outside the array of the character.
    int cp=is_pos+1;
    int c=0;
    while (cp < end_pos)
    {
      number[c] = result[cp];
      c++;
      cp++;
      if (c > 9)
        break;
    }
    return atoi(number);
  }
  // nothing here, so we return NULL at the end
 }

 return NULL; // No value, return NULL
}


void INI_WordValueSENTENCE(char result[MAX_LINE_LENGTH], char value[256])
{
 int pos=0;
 int is_pos=-1;

 // clear out entire string
 memset(value, 0, sizeof(value));

 for (int i=0; i < MAX_LINE_LENGTH;i++)    
     value[i] = '\0';

 
 while (pos < (MAX_LINE_LENGTH-1))
 {
  if (result[pos] == '"')
  {
    is_pos=pos;
    break;
  }
    pos++;
 }
 
 if (is_pos > -1)
 {
   // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
   // space.
   int end_pos=-1;
   pos++;

  while (pos < (MAX_LINE_LENGTH-1))
  {
    if (result[pos] == '"')
    {
      end_pos=pos;
      break;
    }
      pos++;
  } 
  
  // End position found!
  if (end_pos > -1)
  {
    // We know the END position. We will use that piece of string to read out a number.
    
    // Copy the part to 'value', Make sure we won't get outside the array of the character.
    int cp=is_pos+1;
    int c=0;
    while (cp < end_pos)
    {
      value[c] = result[cp];
      c++;
      cp++;
      if (c > 254)
        break;
    }    
  } 
 } 
}

// Reads out 'result' and will return the value after the '='. Returns nothing but will put
// the result in 'value[25]'. Max argument may be 256 characters!
void INI_WordValueCHAR(char result[MAX_LINE_LENGTH], char value[256])
{
 int pos=0;
 int is_pos=-1;

 // clear out entire string
 memset(value, 0, sizeof(value));

 //for (int i=0; i < MAX_LINE_LENGTH;i++)    
  //value[i] = '\0';

 
 while (pos < (MAX_LINE_LENGTH-1))
 {
  if (result[pos] == '=')
  {
    is_pos=pos;
    break;
  }
    pos++;
 }
 
 if (is_pos > -1)
 {
   // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
   // space.
   int end_pos=-1;

  while (pos < (MAX_LINE_LENGTH-1))
  {
    if (result[pos] == '\0')
    {
      end_pos=pos;
      break;
    }
      pos++;
  } 
  
  // End position found!
  if (end_pos > -1)
  {
    // We know the END position. We will use that piece of string to read out a number.
    
    // Copy the part to 'value', Make sure we won't get outside the array of the character.
    int cp=is_pos+1;
    int c=0;
    while (cp < end_pos)
    {
      value[c] = result[cp];
      c++;
      cp++;
      if (c > 80)
        break;
    }
  } 
 
 } 

 
}


// Reads out 'result' and will return TRUE when its 'TRUE' or FALSE when its 'FALSE' , else
// returns NULL
bool INI_WordValueBOOL(char result[MAX_LINE_LENGTH])
{
  // use INI_WordValueCHAR to know if its 'true'
  char val[256];
  INI_WordValueCHAR(result, val);
  
  // When its TRUE , return true
  if (strcmp(val, "TRUE") == 0)
    return true;

  // When its FALSE , return false
  if (strcmp(val, "FALSE") == 0)
    return false;

  // Return NULL (no answer, or we cannot identify it as a BOOL)
  return NULL;  
}
// return ID of structure
int INI_Structure_ID(char *structure)
{
	if (strcmp(structure, "Const Yard") == 0)
		return CONSTYARD;

	if (strcmp(structure, "Palace") == 0)
		return PALACE;

	if (strcmp(structure, "Heavy Fctry") == 0)
		return HEAVYFACTORY;

	if (strcmp(structure, "Light Fctry") == 0)
		return LIGHTFACTORY;

	if (strcmp(structure, "Windtrap") == 0)
		return WINDTRAP;

	if (strcmp(structure, "Spice Silo") == 0)
		return SILO;

	if (strcmp(structure, "Hi-Tech") == 0)
		return HIGHTECH;

	if (strcmp(structure, "IX") == 0)
		return IX;

	if (strcmp(structure, "Repair") == 0)
		return REPAIR;

	if (strcmp(structure, "Outpost") == 0)
		return RADAR;

	if (strcmp(structure, "Refinery") == 0)
		return REFINERY;

	if (strcmp(structure, "WOR") == 0)
		return WOR;

	if (strcmp(structure, "Barracks") == 0)
		return BARRACKS;

	if (strcmp(structure, "Starport") == 0)
		return STARPORT;

	if (strcmp(structure, "Turret") == 0)
		return TURRET;

	if (strcmp(structure, "R-Turret") == 0)
		return RTURRET;

	logbook("Could not find structure:");
	logbook(structure);
	return CONSTYARD;
}

// return ID of unit
int INI_Unit_ID(char *unit)
{
	if (strcmp(unit, "Quad") == 0)
		return QUAD;

	if (strcmp(unit, "MCV") == 0)
		return MCV;

	if (strcmp(unit, "Trike") == 0)
		return TRIKE;
	
	if (strcmp(unit, "Raider Trike") == 0)
		return RAIDER;
	
	if (strcmp(unit, "Trooper") == 0)
		return TROOPER;
	
	if (strcmp(unit, "Soldier") == 0)
		return SOLDIER;

	if (strcmp(unit, "Troopers") == 0)
		return TROOPERS;

	if (strcmp(unit, "Tank") == 0)
		return TANK;

	if (strcmp(unit, "Launcher") == 0)
		return LAUNCHER;

	if (strcmp(unit, "Sonic Tank") == 0)
		return SONICTANK;

	if (strcmp(unit, "Siege Tank") == 0)
		return SIEGETANK;

	if (strcmp(unit, "Infantry") == 0)
		return INFANTRY;

	if (strcmp(unit, "Devastator") == 0)
		return DEVASTATOR;

	if (strcmp(unit, "Deviator") == 0)
		return DEVIATOR;

	if (strcmp(unit, "Harvester") == 0)
		return HARVESTER;

	if (strcmp(unit, "CarryAll") == 0)
		return CARRYALL;

	if (strcmp(unit, "Ornithopter") == 0)
		return ORNITHOPTER;

	if (strcmp(unit, "Sandworm") == 0)
		return SANDWORM;

	if (strcmp(unit, "Saboteur") == 0)
		return SABOTEUR;    

	logbook("INI (WARNING) : Could not find unit:");
	logbook(unit);
	return QUAD;
}


// loads dunedit seed file
// After all these years, Dunedit still is 'needed' somehow.
void INI_Load_seed(int seed)
{
	char filename[50];
	for (int ic=0; ic < 50; ic++)
		filename[ic] = '\0';

	sprintf(filename, "campaign\\maps\\seed\\%d.map", seed);

	FILE *stream; 
	int iY=1;

	char msg[256];
	sprintf(msg, "[SEED] NR %d.\n[SEED] LOCATION '%s'", seed, filename);
	logbook(msg);

	logbook("[SEED] Opening file");

	if( (stream = fopen( filename, "r+t" )) != NULL )
	{
	
		char linefeed[MAX_LINE_LENGTH];
		
		// infinite loop baby
		while( !feof( stream ) )
		{
			INI_Sentence(stream, linefeed);

			// each line is 64 chars. In fact 62 chars are used.
			for (int iX=0; iX < 62; iX++)
			{
				int iCell = iCellMake((iX+1), (iY));

				char letter[1];
				letter[0] = '\0';
				letter[0] = linefeed[iX];
				
				if (letter[0] == '%')                
                    map.create_spot(iCell, TERRAIN_ROCK, 0);

				if (letter[0] == '^')
					map.create_spot(iCell, TERRAIN_ROCK, 0);

                if (letter[0] == '&')
					map.create_spot(iCell, TERRAIN_ROCK, 0);
                
                if (letter[0] == '(')
					map.create_spot(iCell, TERRAIN_ROCK, 0);
                
				if (letter[0] == 'R')
				{
					map.create_spot(iCell, TERRAIN_MOUNTAIN, 0);
				}
				if (letter[0] == 'r')
				{
					map.create_spot(iCell, TERRAIN_MOUNTAIN, 0);
				}
				if (letter[0] == '+')
				{
                    map.create_spot(iCell, TERRAIN_SPICEHILL, 0);					
				}
				if (letter[0] == '-')
				{
					map.create_spot(iCell, TERRAIN_SPICE, 0);
				}

				// HILLS (NEW)
				if (letter[0] == 'H')
					map.create_spot(iCell, TERRAIN_HILL, 0);

				if (letter[0] == 'h')
					map.create_spot(iCell, TERRAIN_HILL, 0);
                
			}
			iY++;
			if (iY > 61)
				break;
		}
		logbook("[SEED] Done.");
		fclose(stream);
	} // eof

}


// Load

void INI_Load_Regionfile(int iHouse, int iMission)
{
	char filename[80];
	char cHouse[15];
	
    memset(filename, 0, sizeof(filename));
    memset(cHouse, 0, sizeof(cHouse));

	if (iHouse == ATREIDES)	sprintf(cHouse, "atreides");
	if (iHouse == HARKONNEN)	sprintf(cHouse, "harkonnen");
	if (iHouse == SARDAUKAR)	sprintf(cHouse, "sardaukar");
	if (iHouse == ORDOS)	sprintf(cHouse, "ordos");
	if (iHouse == FREMEN)	sprintf(cHouse, "fremen");
	if (iHouse == MERCENARY)	sprintf(cHouse, "mercenary");
	
	sprintf(filename, "campaign\\%s\\mission%d.ini", cHouse, iMission);

	char msg[256];
	sprintf(msg, "[CAMPAIGN] '%s' (Mission %d)", filename, game.iMission);	
	logbook(msg);

	
  ////////////////////////////
  // START OPENING FILE
  ////////////////////////////
	FILE *stream;					// file stream
	int wordtype=WORD_NONE;			// word
	int iRegionIndex=-1;
    int iRegionNumber=-1;
    int iRegionConquer=-1;

  if( (stream = fopen( filename, "r+t" )) != NULL )
  {
	  logbook("[CAMPAIGN] Opening file"); // make note on logbook

    char linefeed[MAX_LINE_LENGTH];
    char lineword[25];
    char linesection[30];

	for (int iCl=0; iCl < MAX_LINE_LENGTH; iCl++)
	{
		linefeed[iCl] = '\0';
		if (iCl < 25) lineword[iCl] = '\0';
		if (iCl < 30) linesection[iCl] = '\0';
	}

    // infinite loop baby
    while( !feof( stream ) )
    {
      INI_Sentence(stream, linefeed);

      // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (linefeed[0] == ';' ||
          linefeed[0] == '#' ||
         (linefeed[0] == '/' && linefeed[1] == '/') ||
          linefeed[0] == '\n' ||
          linefeed[0] == '\0')
          continue;   // Skip

	  wordtype=WORD_NONE;

    // Every line is checked for a new section.
	  INI_Word(linefeed, lineword);
	  wordtype = INI_WordType(lineword, SEC_REGION);

	  if (wordtype == WORD_REGION)
	  {	
          iRegionNumber=-1;
          iRegionConquer=-1;
		  iRegionNumber = INI_WordValueINT(linefeed)-1;          
	  }

      else if (wordtype == WORD_REGIONCONQUER)
	  {	
          iRegionNumber=-1;
          iRegionConquer=-1;
		  iRegionIndex++;
		  iRegionConquer = INI_WordValueINT(linefeed)-1;
          game.iRegionConquer[iRegionIndex] = iRegionConquer;          
	  }

	  if (iRegionIndex > -1 || iRegionNumber > -1)
	  {
		if (wordtype == WORD_REGIONHOUSE)
		{
			char cHouseRegion[256];
			memset(cHouseRegion, 0, sizeof(cHouseRegion));
			INI_WordValueCHAR(linefeed, cHouseRegion);

			logbook("Region house");
            int iH=-1;

			logbook(cHouseRegion);
			if (strcmp(cHouseRegion, "Atreides") == 0)     iH   = ATREIDES;
			if (strcmp(cHouseRegion, "Harkonnen") == 0)    iH   = HARKONNEN;
			if (strcmp(cHouseRegion, "Ordos") == 0)        iH   = ORDOS;
			if (strcmp(cHouseRegion, "Sardaukar") == 0)    iH   = SARDAUKAR;
			if (strcmp(cHouseRegion, "Fremen" ) == 0)      iH   = FREMEN;
			if (strcmp(cHouseRegion, "Mercenary") == 0)    iH   = MERCENARY;

            if (iRegionNumber > -1)
            {
                world[iRegionNumber].iHouse = iH;
                world[iRegionNumber].iAlpha = 255;                
            }

            if (iRegionConquer > -1)
                game.iRegionHouse[iRegionIndex] = iH;

		}

        if (wordtype == WORD_REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1)
        {
			char cHouseText[256];			
            INI_WordValueSENTENCE(linefeed, cHouseText);
            sprintf(game.cRegionText[iRegionIndex], "%s", cHouseText);            
        }

        if (wordtype == WORD_REGIONSELECT)
        {
            if (iRegionNumber > -1)
                world[iRegionNumber].bSelectable = INI_WordValueBOOL(linefeed);
        }

	  }
	}

   fclose(stream); 
   logbook("[CAMPAIGN] Done"); // make note on logbook	
  }
  else
	  logbook("[CAMPAIGN] Error, could not open file"); // make note on logbook	
	

}

// SCENxxxx.ini loader (for both DUNE II as for DUNE II - The Maker)
void INI_Load_scenario(int iHouse, int iRegion)
{

	// Always set to false (TODO: undo this, but this is now for debugging purposes
	// hot jumping to missions from skirmish mode into scenario mode).
	game.bSkirmish=false;

	// First initialize the map data
	//map.init(); 

	game.mission_init();

	// Start assembling file name for loading
	char cHouse[4];
	memset(cHouse, 0, sizeof(cHouse));
  
	
	if (iHouse == ATREIDES) sprintf(cHouse, "a");
	if (iHouse == HARKONNEN) sprintf(cHouse, "h");  
	if (iHouse == ORDOS) sprintf(cHouse, "o");
	if (iHouse == SARDAUKAR) sprintf(cHouse, "s");
	if (iHouse == MERCENARY) sprintf(cHouse, "m");
	if (iHouse == FREMEN) sprintf(cHouse, "f");

	char filename[60];
	
	if (iRegion < 10)
		sprintf(filename, "campaign//maps//Scen%s00%d.ini", cHouse, iRegion);
	else
		sprintf(filename, "campaign//maps//Scen%s0%d.ini", cHouse, iRegion);


    

	// Done assembling. Now calculate the mission (techlevel) out of the region

    // SEEN FROM HOUSE PERSPECTIVE! (ie, first mission starts just that, 2nd is choosen, and is culumative)

    // MISSION = TECHLEVEL
    // REGION = SCEN*NR

	// Calculate mission from region:
	// region 1 = mission 1
    // region 2, 3, 4 = mission 2
    // region 5, 6, 7 = mission 3
    // region 8, 9, 10 = mission 4
    // region 11,12,13 = mission 5
    // region 14,15,16 = mission 6
    // region 17,18,19 = mission 7
    // region 20,21    = mission 8
	// region 22 = mission 9

	if (iRegion == 1)
		game.iMission = 1;
	else if (iRegion == 2 || iRegion == 3 || iRegion == 4)
		game.iMission = 2;
	else if (iRegion == 5 || iRegion == 6 || iRegion == 7)
		game.iMission = 3;
	else if (iRegion == 8 || iRegion == 9 || iRegion == 10)
		game.iMission = 4;
	else if (iRegion == 11 || iRegion == 12 || iRegion == 13)
		game.iMission = 5;
	else if (iRegion == 14 || iRegion == 15 || iRegion == 16)
		game.iMission = 6;
	else if (iRegion == 17 || iRegion == 18 || iRegion == 19)
		game.iMission = 7;
	else if (iRegion == 20 || iRegion == 21)
		game.iMission = 8;
	else if (iRegion == 22)
		game.iMission = 9;



	// Open up the file and read data.

	char msg[256];
	sprintf(msg, "[SCENARIO] '%s' (Mission %d)", filename, game.iMission);	
	logbook(msg);

	logbook("[SCENARIO] Opening file"); // make note on logbook
	//logbook(filename);
  
	// Dune 2 scenario loading requires us to remember player data first and later to set up
	// this is done at the UNITS section, when everything has been preloaded.

	// Array list for spice blooms
	// Blooms & fields are loaded in memory first (from the MAP section), and after the actual SEED loading (map data)
	// we can apply these. (since FIELD and BLOOM vars are loaded BEFORE the SEED data)
	int blooms[30];
	int fields[30];

    char value[256];

	memset(blooms, -1, sizeof(blooms));
	memset(fields, -1, sizeof(fields));
	//for (int iB=0; iB < 30; iB++)
	//	blooms[iB]=-1;    // reset array

	// Load file
	FILE *stream;					// file stream
	int section=INI_NONE;			// section
	int wordtype=WORD_NONE;			// word
	int iPlayerID=-1;				// what player are we loading data for?
	int iHumanID=-1;				// what player ID is the same as our HUMAN (0) id?
	bool bSetUpPlayers=true;		// may we set up player data when entering the UNITS section?

  
	// Preloaded player data
	int iPl_credits[MAX_PLAYERS];
	int iPl_house[MAX_PLAYERS];
    int iPl_quota[MAX_PLAYERS]; // get quota
  
	
	// clear info
	for (int i=0; i < MAX_PLAYERS; i++)  
	{
		iPl_credits[i] = 0;	    
		iPl_house[i]=-1;
        iPl_quota[i]=0;
	}

  ////////////////////////////
  // START OPENING FILE
  ////////////////////////////

  if( (stream = fopen( filename, "r+t" )) != NULL )
  {
    char linefeed[MAX_LINE_LENGTH];
    char lineword[25];
    char linesection[30];

	for (int iCl=0; iCl < MAX_LINE_LENGTH; iCl++)
	{
		linefeed[iCl] = '\0';
		if (iCl < 25) lineword[iCl] = '\0';
		if (iCl < 30) linesection[iCl] = '\0';
	}

    // infinite loop baby
    while( !feof( stream ) )
    {
      INI_Sentence(stream, linefeed);

      // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (linefeed[0] == ';' ||
          linefeed[0] == '#' ||
         (linefeed[0] == '/' && linefeed[1] == '/') ||
          linefeed[0] == '\n' ||
          linefeed[0] == '\0')
          continue;   // Skip

      // Every line is checked for a new section.
      INI_Section(linefeed,linesection);

      if (linesection[0] != '\0' && strlen(linesection) > 1)
      {
        section= SCEN_INI_SectionType(linesection, section);

		// Only original dune 2 scenario's have this section, auto set to true
		if (section == INI_BASIC)
        {			
            //logbook("NOTE: found '[BASIC]' section. Meaning this is a DUNE II scenario afterall...");

			// Read out tactical CELL here... "TODO"

            // READ OUT STARTING MOVIE

        }
        
        if (section >= INI_HOUSEATREIDES &&
            section <= INI_HOUSEMERCENARY)
        {
                iPlayerID++;

		if (iPlayerID > (MAX_PLAYERS-1))
            iPlayerID = (MAX_PLAYERS-1);
                
        if (section == INI_HOUSEATREIDES)   iPl_house[iPlayerID] = ATREIDES;
        if (section == INI_HOUSEORDOS)      iPl_house[iPlayerID] = ORDOS;
        if (section == INI_HOUSEHARKONNEN)  iPl_house[iPlayerID] = HARKONNEN;
        if (section == INI_HOUSEMERCENARY)  iPl_house[iPlayerID] = MERCENARY;
        if (section == INI_HOUSEFREMEN)     iPl_house[iPlayerID] = FREMEN;
        if (section == INI_HOUSESARDAUKAR)  iPl_house[iPlayerID] = SARDAUKAR;
 
		}
        continue; // next line
      }
         // Okay, we found a new section; if its NOT [GAME] then we remember this one!
		if (section != INI_NONE)
		{
			INI_Word(linefeed, lineword);
			wordtype = INI_WordType(lineword, section);
		}

        if (section == INI_BASIC)
        {
            if (wordtype == WORD_BRIEFPICTURE)
            {
                // Load name, and load proper briefingpicture               
                memset(value, 0, sizeof(value));
                
                INI_WordValueCHAR(linefeed, value); 

                gfxmovie=NULL;
                game.iMovieFrame=-1;

                // wsa / data 
                if (strcmp(value, "HARVEST.WSA") == 0)
                   LOAD_SCENE("harvest"); // load harvester

                if (strcmp(value, "IX.WSA") == 0)
                   LOAD_SCENE("ix"); // load harvester

                if (strcmp(value, "SARDUKAR.WSA") == 0)
                   LOAD_SCENE("sardukar"); // load harvester

                if (strcmp(value, "PALACE.WSA") == 0)
                   LOAD_SCENE("palace"); // load harvester

                if (strcmp(value, "REPAIR.WSA") == 0)
                   LOAD_SCENE("repair"); // repair facility

                if (strcmp(value, "HVYFTRY.WSA") == 0)
                   LOAD_SCENE("hvyftry"); // load heavy factory

                if (strcmp(value, "HEADQRTS.WSA") == 0)
                   LOAD_SCENE("headqrts"); // radar/headquarters

                if (strcmp(value, "QUAD.WSA") == 0)
                   LOAD_SCENE("quad"); // quad

                if (strcmp(value, "LTANK.WSA") == 0)
                   LOAD_SCENE("ltank"); // ltank


                //assert(value);

            }            
        }

		// Dune 2 house found, load player data
		if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY)
		{
			// link house
			if (iPlayerID > -1)
			{
				if (wordtype == WORD_BRAIN)
				{
					char cBrain[256];
                    memset(cBrain, 0, sizeof(cBrain));
					INI_WordValueCHAR(linefeed, cBrain);

					// We know the human brain now, this should be player 0...
					if (strcmp(cBrain, "Human") == 0)
						iHumanID = iPlayerID;                    

				}

				if (wordtype == WORD_CREDITS)										
					iPl_credits[iPlayerID] = INI_WordValueINT(linefeed)-1;

                if (wordtype == WORD_QUOTA)
                {
					iPl_quota[iPlayerID] = INI_WordValueINT(linefeed);
                }

                
			}

		}


        // [BASIC]
		if (section == INI_BASIC)
		{
			if (wordtype == WORD_FOCUS)
			{
                player[0].focus_cell = INI_WordValueINT(linefeed);
				map.set_pos(-1,-1, player[0].focus_cell);
			}
		}

        if (section == INI_MAP)
		{
			game.map_height = 64;
			game.map_width = 64;
			
			// original dune 2 maps have 64x64 maps
            if (wordtype == WORD_MAPSEED)
			{
				logbook("[SCENARIO] -> [MAP] Seed="); 
				INI_Load_seed(INI_WordValueINT(linefeed));
			}

			// Loaded before SEED
            if (wordtype == WORD_MAPBLOOM)
			{
                // This should put spice blooms in our array
                // Logic: read till next "," , then use that number to determine
                // where the bloom will be (as cell nr)
				logbook("[SCENARIO] -> [MAP] Bloom="); 

                int iBloomID=0;				
                int iStringID=6;    // B L O O M = <6>
                int iWordID=0;

                char word[10];
                memset(word, 0, sizeof(word)); // clear string

                for (iStringID; iStringID < MAX_LINE_LENGTH; iStringID++)
                {
                    // until we encounter a "," ...

                    char letter[1];
                    letter[0] = '\0';
                    letter[0] = linefeed[iStringID];

                    // its a comma!
                    if (letter[0] == ',' || letter[0] == '\0' || letter[0] == '\n')
                    {
                        // from prevID TILL now is a number                       
                        iWordID=0;

                        int original_dune2_cell = atoi(word);
                        int d2tm_cell = -1;

                        int iCellX=(original_dune2_cell - ((original_dune2_cell/64) * 64));
                        int iCellY=(original_dune2_cell / 64);

                        // Now recalculate it
                        d2tm_cell = iCellMake(iCellX, iCellY);                       
                        blooms[iBloomID] = d2tm_cell;                        
                        memset(word, 0, sizeof(word)); // clear string

                        if (iBloomID < 29)
                            iBloomID++;

                        if (letter[0] == '\0' || letter[0] == '\n')
                            break; // get out
                    }
                    else
                    {
                        word[iWordID] = letter[0]; // copy
                        if (iWordID < 9)
                            iWordID++;
                        
                    }
                }

                
			}
			// Loaded before SEED
        else if (wordtype == WORD_MAPFIELD)
			{
                // This should put spice blooms in our array
                // Logic: read till next "," , then use that number to determine
                // where the bloom will be (as cell nr)
				
			logbook("[SCENARIO] -> [MAP] Field="); 
                int iFieldID=0;
                int iStringID=6;    // F I E L D = <6>
                int iWordID=0;

                char word[10];
                memset(word, 0, sizeof(word)); // clear string

                for (iStringID; iStringID < MAX_LINE_LENGTH; iStringID++)
                {
                    // until we encounter a "," ...

                    char letter[1];
                    letter[0] = '\0';
                    letter[0] = linefeed[iStringID];

                    // its a comma!
                    if (letter[0] == ',' || letter[0] == '\0' || letter[0] == '\n')
                    {
                        // from prevID TILL now is a number                       
                        iWordID=0;

                        int original_dune2_cell = atoi(word);
                        int d2tm_cell = -1;

                        int iCellX=(original_dune2_cell - ((original_dune2_cell/64) * 64));
                        int iCellY=(original_dune2_cell / 64);

                        // Now recalculate it
                        d2tm_cell = iCellMake(iCellX, iCellY);                       
                        fields[iFieldID] = d2tm_cell;                        
                        memset(word, 0, sizeof(word)); // clear string

                        if (iFieldID < 29)
                            iFieldID++;

                        if (letter[0] == '\0' || letter[0] == '\n')
                            break; // get out
                    }
                    else
                    {
                        word[iWordID] = letter[0]; // copy
                        if (iWordID < 9)
                            iWordID++;
                        
                    }
                }

                
			}
		
		}		
		else if (section == INI_UNITS)
		{  			

            // ORIGINAL DUNE 2 MISSION. EVERYBODY IS AGAINST U
			if (bSetUpPlayers)
			{                
                int iRealID=1;
                
                for (int iP=0; iP < MAX_PLAYERS; iP++) // till 6 , since player 6 itself is sandworm
                {
                    if (iPl_house[iP] > -1)
                        if (iP == iHumanID)
                        {
							player[0].credits = iPl_credits[iP];
							player[0].draw_credits = 	player[0].credits;
							player[0].set_house(iPl_house[iP]);
                            player[0].iTeam=0;
                            game.iHouse = iPl_house[iP];
                            
                            if (iPl_quota[iP] > 0)
                                game.iWinQuota = iPl_quota[iP];
						}
						else
						{
                            // CPU player
                            player[iRealID].iTeam = 1; // All AI players are on the same team

                            // belong to player team
                            if (iPl_house[iP] == FREMEN)
                                if (player[0].house == ATREIDES)
                                    player[iRealID].iTeam = 0;

							player[iRealID].credits = iPl_credits[iP];							
							player[iRealID].set_house(iPl_house[iP]);							
							iRealID++;
						}						
					}
					bSetUpPlayers=false;				
            }

			int iPart=-1; /*
							0 = Controller
							1 = Type
							2 = HP
							3 = Cell
							4 = Facing (body)
							5 = Facing (head)
							*/

			// Skip ID= part. It is just for fun there.
			int iController, iType, iHP, iCell, iFacingBody, iFacingHead;
			iController=iType=iHP=iCell=iFacingBody=iFacingHead=-1;

			char chunk[25];
			bool bClearChunk=true;
			bool bSkipped=false;
			int iC=-1;
			
			for (int c=0; c < MAX_LINE_LENGTH; c++)
			{
				// clear chunk
				if (bClearChunk)
				{
					for (int ic=0; ic < 25; ic++)
						chunk[ic] = '\0';
					iC=0;
					bClearChunk=false;
				}				

				// Fill in chunk
				if (iC < 25 && bSkipped && linefeed[c] != ',')
				{
					chunk[iC] = linefeed[c];
					iC++;
				}

				// , means next part. A ' ' means end
				if (linefeed[c] == ',' || linefeed[c] == '\0')
				{					
					iPart++;
				
						if (iPart == 0)
						{
							int iHouse=-1;

							if (strcmp(chunk, "Atreides") == 0)     iHouse = ATREIDES;
							if (strcmp(chunk, "Harkonnen") == 0)     iHouse = HARKONNEN;
							if (strcmp(chunk, "Ordos") == 0)     iHouse = ORDOS;
							if (strcmp(chunk, "Sardaukar") == 0)     iHouse = SARDAUKAR;
							if (strcmp(chunk, "Mercenary") == 0)     iHouse = MERCENARY;
							if (strcmp(chunk, "Fremen") == 0)     iHouse = FREMEN;
							if (strcmp(chunk, "Corrino") == 0)     iHouse = CORRINO;
							if (strcmp(chunk, "General") == 0)     iHouse = GENERALHOUSE;

							// Search for a player with this house						
							for (int i=0; i < MAX_PLAYERS; i++)
								if (player[i].house == iHouse)
								{
									iController = i; // set controller here.. phew
									break;
								}

								// HACK HACK : Set up fremen house here
							if (iHouse == FREMEN)
							{	
								player[5].set_house(FREMEN); // set up palette
								player[5].house = FREMEN;
								player[5].credits = 9999; // lots of money for the fremen							

								iController = 5;
							}

							
							// Quickfix: fremen house in original dune 2, is house 6 (not detectable)
							// in this game.
							//if (iHouse == FREMEN)
							//	iController = 6;

							if (iController < 0)
							{
                                char msg[256];
                                sprintf(msg,"WARNING: Cannot identify house/controller -> STRING '%s'", chunk);
                                logbook(msg);								
							}

						}
						else if (iPart == 1)
						{
							iType = INI_Unit_ID(chunk);
						}
						// do nothing in part 2
						else if (iPart == 3)
						{
							iCell = atoi(chunk);
							

						}
						else if (iPart == 4)
							break;

				

					bClearChunk=true;
				}

				// found the = mark, this means we start chopping now!
				if (linefeed[c] == '=')				
					bSkipped=true;
			}

			
			if (iController > -1)
			{
				//int uID = create_unit(iController, iCell, iType, ACTION_GUARD, -1);
                UNIT_CREATE(iCell, iType, iController, true);
			//	CREATE_UNIT(iCell, iType, iController);

                /*
				if (bOrDune == false && uID > -1)
				{
					unit[uID].hp = iHP;
					unit[uID].body_face = iFacingBody;
					unit[uID].body_head = iFacingHead;
					unit[uID].body_should_face = iFacingBody;
					unit[uID].head_should_face = iFacingHead;
				}*/
			}
		}
		else if (section == INI_STRUCTURES)
		{
			// In case some funny-face of Westwood did first the structures section, we still set up players then...
			// har har har..
            // ORIGINAL DUNE 2 MISSION. EVERYBODY IS AGAINST U
			if (bSetUpPlayers)
			{                
                int iRealID=1;
                
                for (int iP=0; iP < MAX_PLAYERS; iP++) // till 6 , since player 6 itself is sandworm
                {
                    if (iPl_house[iP] > -1)
                        if (iP == iHumanID)
                        {
							player[0].credits = iPl_credits[iP];
							player[0].draw_credits = 	player[0].credits;
							player[0].set_house(iPl_house[iP]);
                            player[0].iTeam=0;
                            game.iHouse = iPl_house[iP];
                            
                            if (iPl_quota[iP] > 0)
                                game.iWinQuota = iPl_quota[iP];
						}
						else
						{
                            // CPU player
                            player[iRealID].iTeam = 1; // All AI players are on the same team

                            // belong to player team
                            if (iPl_house[iP] == FREMEN)
                                if (player[0].house == ATREIDES)
                                    player[iRealID].iTeam = 0;

							player[iRealID].credits = iPl_credits[iP];							
							player[iRealID].set_house(iPl_house[iP]);							
							iRealID++;
						}						
					}
					bSetUpPlayers=false;				
            }


			int iPart=-1; /*
							0 = Controller
							1 = Type
							2 = HP
							3 = Cell							
							*/

			// Skip ID= part. It is just for fun there.
			int iController, iType, iHP, iCell;
			iController=iType=iHP=iCell=-1;

			char chunk[25];
			bool bClearChunk=true;
			bool bSkipped=false;
			int iC=-1;
			bool bGen=false;
			int iIS=-1;

			// check if this is a 'gen'
			if (strstr(linefeed, "GEN") != NULL) bGen=true;

			for (int c=0; c < MAX_LINE_LENGTH; c++)			
			{
				// clear chunk
				if (bClearChunk)
				{
					for (int ic=0; ic < 25; ic++)
						chunk[ic] = '\0';
					iC=0;
					bClearChunk=false;
				}				
				
				// Fill in chunk
				if (iC < 25 && bSkipped && linefeed[c] != ',')
				{
					chunk[iC] = linefeed[c];
					iC++;
				}


				// , means next part. A ' ' means end
				if (linefeed[c] == ',' || linefeed[c] == '\0')
				{					
					iPart++;
				
						if (bGen == false)
						{
							if (iPart == 0)
							{
								int iHouse=-1;

								if (strcmp(chunk, "Atreides") == 0)     iHouse = ATREIDES;
								if (strcmp(chunk, "Harkonnen") == 0)     iHouse = HARKONNEN;
								if (strcmp(chunk, "Ordos") == 0)     iHouse = ORDOS;
								if (strcmp(chunk, "Sardaukar") == 0)     iHouse = SARDAUKAR;
								if (strcmp(chunk, "Mercenary") == 0)     iHouse = MERCENARY;
								if (strcmp(chunk, "Fremen") == 0)		 iHouse = FREMEN;
								if (strcmp(chunk, "Corrino") == 0)		 iHouse = CORRINO;
								if (strcmp(chunk, "General") == 0)		 iHouse = GENERALHOUSE;
							
								// Search for a player with this house								
								for (int i=0; i < MAX_PLAYERS; i++)
								{
									//char msg[80];
									//sprintf(msg, "i=%d, ihouse=%d, house=%d", i, iHouse, player[i].house);
									//logbook(msg);
									if (player[i].house == iHouse)
									{
										iController = i; // set controller here.. phew
										break;
									}
								}
								// Quickfix: fremen house in original dune 2, is house 6 (not detectable)
								// in this game.
								//if (iHouse == FREMEN)
								//	iController = 6;

								if (iController < 0)
								{
									logbook("WARNING: Identifying house/controller of structure (typo?)");
									logbook(chunk);
								}
							}
							else if (iPart == 1)
							{
								iType = INI_Structure_ID(chunk);
							}
							else if (iPart == 3)
							{
								iCell = atoi(chunk);								
								
								break;
							}
						}
						else
						{
							if (iPart == 0)
							{
								int iHouse=-1;

								if (strcmp(chunk, "Atreides") == 0)     iHouse = ATREIDES;
								if (strcmp(chunk, "Harkonnen") == 0)     iHouse = HARKONNEN;
								if (strcmp(chunk, "Ordos") == 0)     iHouse = ORDOS;
								if (strcmp(chunk, "Sardaukar") == 0)     iHouse = SARDAUKAR;
								if (strcmp(chunk, "Mercenary") == 0)     iHouse = MERCENARY;
								if (strcmp(chunk, "Fremen") == 0)     iHouse = FREMEN;
								if (strcmp(chunk, "Corrino") == 0)     iHouse = CORRINO;
								if (strcmp(chunk, "General") == 0)     iHouse = GENERALHOUSE;

								// Search for a player with this house							
								for (int i=0; i < MAX_PLAYERS; i++)
								{
									if (player[i].house == iHouse)
									{
										iController = i; // set controller here.. phew
										break;
									}
								}
							}
							else if (iPart == 1)
							{
								// Figure out the cell shit of this GEN
								char cCell[5];
								for (int cc=0; cc < 5; cc++)
									cCell[cc] = '\0';

								int iCC=0;
								for (cc=3; cc < iIS; cc++)
								{
									cCell[iCC] = linefeed[cc];
									iCC++;
								}
																
								int iGenCell = atoi(cCell);	
								
								iCell = iGenCell;

								if (strcmp(chunk, "Wall") == 0) iType = WALL;
								if (strcmp(chunk, "Concrete") == 0) iType = SLAB1;
								break;
							}
						}
					
					bClearChunk=true;
				}

				// found the = mark, this means we start chopping now!
				if (linefeed[c] == '=')
				{
					bSkipped=true;
					iIS=c;
				}
			}

			if (iController > -1)
            {
				if (iType < SLAB1)
				{
					CREATE_STRUCTURE(iCell, iType, iController, 100);
					//sID = STRUCTURE_CREATE(iCell, iType, -1, iController);
				}                 
				else
				{
					if (iType == SLAB1) 
                    {
                        map.create_spot(iCell, TERRAIN_SLAB, 0);
                        //map.cell[iCell].tile = SLAB;
                    }
					if (iType == WALL) 
					{
					    map.create_spot(iCell, TERRAIN_WALL, 0);
					}
				}				
			}
			else logbook("WARNING: Identifying house/controller of structure (typo?)");
		}
        else if (section == INI_REINFORCEMENTS)
        {
            logbook("[SCENARIO] -> REINFORCEMENTS");

            	int iPart=-1; /*
							0 = Controller
							1 = Type
							2 = HP
							3 = Cell							
							*/

			// Skip ID= part. It is just for fun there.
			int iController, iType, iTime, iCell;
			iController=iType=iTime=iCell=-1;

			char chunk[25];
			bool bClearChunk=true;
			bool bSkipped=false;
			int iC=-1;
			int iIS=-1;


			for (int c=0; c < MAX_LINE_LENGTH; c++)			
			{
				// clear chunk
				if (bClearChunk)
				{
					memset(chunk, 0, sizeof(chunk));
					//for (int ic=0; ic < 25; ic++)
					//	chunk[ic] = '\0';
					iC=0;
					bClearChunk=false;
				}				
				
				// Fill in chunk
				if (iC < 25 && bSkipped && linefeed[c] != ',')
				{
					chunk[iC] = linefeed[c];
					iC++;
				}

				// , means next part. A ' ' means end
				if (linefeed[c] == ',' || linefeed[c] == '\0' || linefeed[c] == '+')
				{					
					iPart++;				
                    
                    if (iPart == 0)
                    {
                        int iHouse=-1;
						
                        if (strcmp(chunk, "Atreides") == 0)    iHouse = ATREIDES;
                        if (strcmp(chunk, "Harkonnen") == 0)   iHouse = HARKONNEN;
                        if (strcmp(chunk, "Ordos") == 0)       iHouse = ORDOS;
                        if (strcmp(chunk, "Sardaukar") == 0)   iHouse = SARDAUKAR;
                        if (strcmp(chunk, "Mercenary") == 0)   iHouse = MERCENARY;
                        if (strcmp(chunk, "Fremen") == 0)      iHouse = FREMEN;
                        if (strcmp(chunk, "Corrino") == 0)     iHouse = CORRINO;
                        if (strcmp(chunk, "General") == 0)     iHouse = GENERALHOUSE;

						if (iHouse > -1)
						{
							// Search for a player with this house							
							for (int i=0; i < MAX_PLAYERS; i++)
							{                            
								if (player[i].house == iHouse)
								{
									iController = i; // set controller here.. phew
									break;
								}
							}
						}
					}
                    else if (iPart == 1)
                    {
                        // what to transport?

                        if (strcmp(chunk, "Harvester") == 0)   iType = HARVESTER;
                        if (strcmp(chunk, "Tank") == 0)   iType = TANK;
                        if (strcmp(chunk, "Siege Tank") == 0)   iType = SIEGETANK;
                        if (strcmp(chunk, "Launcher") == 0)   iType = LAUNCHER;
                        if (strcmp(chunk, "Trooper") == 0)   iType = TROOPER;
                        if (strcmp(chunk, "Troopers") == 0)   iType = TROOPERS;
                        if (strcmp(chunk, "Sonic Tank") == 0)   iType = SONICTANK;
                        if (strcmp(chunk, "Quad") == 0)   iType = QUAD;
                        if (strcmp(chunk, "Trike") == 0)   iType = TRIKE;
                        if (strcmp(chunk, "Raider Trike") == 0)   iType = RAIDER;
                        if (strcmp(chunk, "Soldier") == 0)   iType = SOLDIER;
                        if (strcmp(chunk, "Infantry") == 0)   iType = INFANTRY;
                        if (strcmp(chunk, "Devastator") == 0)   iType = DEVASTATOR;
                        if (strcmp(chunk, "Deviator") == 0)   iType = DEVIATOR;

                    }
                    else if (iPart == 2)
                    {
                        // Homebase is home of that house
                        if (strcmp(chunk, "Homebase") == 0)
                        {							
                            iCell = player[iController].focus_cell;
							//logbook("HOMEBASE");
                        }
                        else                        
                        {
                            // enemy base
                            
							if (iController == 0)
							{
                            // Find corresponding house and get controller
                            for (int i=0; i < MAX_PLAYERS; i++)
                                if (player[i].house == iHouse && i != iController)
                                {
                                    iCell = player[i].focus_cell;
									//logbook("ENEMYBASE");
									break;
                                }
							}
							else
							{
								// computer player must find enemy = human
								iCell = player[0].focus_cell;
							}
                        }
                        
                    }
					else if (iPart == 3)
					{
						// Figure out the cell shit of this GEN						
						char cCell[5];
						memset(cCell, 0, sizeof(cCell));
						
						/*
                        int iCC=0;
						for (cc=3; cc < iIS; cc++)
						{
							cCell[iCC] = linefeed[cc];
							iCC++;
						}*/
														
						int iGenCell = atoi(chunk);	
						
						iTime = iGenCell;
                        
						
						SET_REINFORCEMENT(iCell, iController, iTime, iType);
						break;						
					}

					bClearChunk=true;					
				}

				// found the = mark, this means we start chopping now!
				if (linefeed[c] == '=')
				{
					bSkipped=true;
					iIS=c;
				}

                
			}
        }

    
      wordtype=WORD_NONE;

      }              
    
	  fclose(stream);
	// When loading an original dune 2 scenario, we do a trick now to make the
	// players start okay.    

	// Now apply map settings to all players
	for (int iP=0; iP < MAX_PLAYERS; iP++)
	{	
       		// minimum of 1000 credits storage per level
			if (player[iP].max_credits < 1000)
				player[iP].max_credits = 1000;

        
	}

    map.smooth();

    // now add the spice blooms! :)
    for (int iB=0; iB < 30; iB++)
    {
          // when 
          if (blooms[iB] > -1)
          {              
           //   map.cell[blooms[iB]].tile = BLOOM;

			  if (DEBUGGING)
			  {
              char msg[256];
              sprintf(msg, "[SCENARIO] Placing spice BLOOM at cell : %d", blooms[iB]);
              logbook(msg);
			  }
			  
			  map.create_spot(blooms[iB], TERRAIN_BLOOM, 0);

          }
      }

     // At this point, show list of unit types
	
	// now add the fields
    for (iB=0; iB < 30; iB++)
    {
          // when 
          if (fields[iB] > -1)
          {             
			  if (DEBUGGING)
			  {
			  char msg[256];
              sprintf(msg, "[SCENARIO] Placing spice FIELD at cell : %d", fields[iB]);
              logbook(msg);
			  }
		  	  map.create_field(TERRAIN_SPICE, fields[iB], 25+(rnd(50)));
		  }
		  
	}

    logbook("[SCENARIO] Done reading");
  }  

  
  player[AI_WORM].iTeam=-2; // worm is nobodies friend.

  game.setup_list(); // set up the list

  // smooth
  map.smooth();

  if (player[0].iStructures[CONSTYARD] < 1)
      game.iActiveList=LIST_NONE;  
}

void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind)
{
    logbook("[BRIEFING] Opening file");

    char filename[80];
    
    if (iHouse == ATREIDES)       sprintf(filename, "mentata.ini");
    if (iHouse == ORDOS)          sprintf(filename, "mentato.ini");
    if (iHouse == HARKONNEN)      sprintf(filename, "mentath.ini");

    FILE *stream; 

    char path[50];


    // clear mentat
    memset(game.mentat_sentence, 0, sizeof(game.mentat_sentence));

    sprintf(path, "campaign\\briefings\\%s", filename);

    logbook(path);

	if (DEBUGGING)
	{
    char msg[255];
    sprintf(msg, "Going to find SCEN ID #%d and SectionID %d", iScenarioFind, iSectionFind);
    logbook(msg);
	}

    int iScenario=0;
    int iSection=0;
    int iLine=0; // max 8 lines


    if( (stream = fopen( path, "r+t" )) != NULL )
    {

        char linefeed[MAX_LINE_LENGTH];
        char lineword[25];
        char linesection[30];

        while( !feof( stream ) )
        { 
            INI_Sentence(stream, linefeed);

                  // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (linefeed[0] == ';' ||
          linefeed[0] == '#' ||
         (linefeed[0] == '/' && linefeed[1] == '/')
          )
          continue;   // Skip

      INI_Section(linefeed,linesection);

      if (linesection[0] != '\0' && strlen(linesection) > 1)              
      {           
          // until we found the right sections/parts, keep searching
          iSection=INI_SectionType(linesection, iSection);            
      }

      if (iSection == INI_SCEN)
      {
         INI_Word(linefeed, lineword);
         int wordtype = INI_WordType(lineword, iSection);

         if (wordtype == WORD_NUMBER)
         {
             iScenario = INI_WordValueINT(linefeed);
             continue;
         }
      }

        if (iScenario == iScenarioFind)
        {
            if (iSection == iSectionFind)
            {
                INI_Word(linefeed, lineword);
        
                int wordtype = INI_WordType(lineword, iSection);
                
                if (wordtype == WORD_REGIONTEXT)
                {
                    char cHouseText[256];
                    INI_WordValueSENTENCE(linefeed, cHouseText);
                    
                    // this is not a comment, add this....
                    sprintf(game.mentat_sentence[iLine], "%s",cHouseText);
                   // logbook(game.mentat_sentence[iLine]);
                    iLine++;
                    
                    if (iLine > 9)
                        break;
                }

            }
        

        }


      /*


  */


        }


    }

    fclose(stream);

    logbook("[BRIEFING] File opened");

}


// Game.ini loader
void INI_Install_Game()
{
  /* 
    Goal of this file:

  */
  logbook("[GAME.INI] Opening file");

  FILE *stream; 
  int section=INI_GAME;
  int wordtype=WORD_NONE;
  int id=-1;

  int side_r, side_b, side_g;
  int team_r, team_g, team_b;   // rgb colors for a team
  side_r=side_g=side_b=-1;
  team_r=team_g=team_b=-1;

  char path[50];
  
  sprintf(path, "game.ini");

  if( (stream = fopen( path, "r+t" )) != NULL )
  {
    char linefeed[MAX_LINE_LENGTH];
    char lineword[25];
    char linesection[30];
    
    // infinite loop baby
    while( !feof( stream ) )
    {
      INI_Sentence(stream, linefeed);

      // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (linefeed[0] == ';' ||
          linefeed[0] == '#' ||
         (linefeed[0] == '/' && linefeed[1] == '/') ||
          linefeed[0] == '\n' ||
          linefeed[0] == '\0')
          continue;   // Skip

      wordtype=WORD_NONE;

      // Every line is checked for a new section.
      INI_Section(linefeed,linesection);

      if (linesection[0] != '\0' && strlen(linesection) > 1)
      { 
        int last=section;
        
        // determine section
        section=GAME_INI_SectionType(linesection, section);

        // When we changed section and its a section with adding ID's. We should reset
        // the ID var
        if (last != section)
        {
          id=-1;
          
          // Show in log file we entered a new section
          if (section == INI_UNITS)       logbook("[GAME.INI] -> [UNITS]");
          if (section == INI_STRUCTURES)  logbook("[GAME.INI] -> [STRUCTURES]");
        }

        if (section == INI_TEAMS)
        {
          // check if we found a new [TEAM part!
          if (strstr(linefeed,"[TEAM:") != NULL)
          {
            id++; // New ID
            team_r=team_g=team_b=-1;    // new team colors
            if (id > MAX_HOUSES) id--;
          }
        }
        
        // New unit type
        if (section == INI_UNITS)
        {
          // check if we found a new [UNIT part!
          if (strstr(linefeed,"[UNIT:") != NULL)
          {
            // Get the name of the unit:
            // [UNIT: <NAME>]
            // 1234567890123...]
            char name_unit[35];
            
            for (int nu=0; nu < 35; nu++)
              name_unit[nu]='\0';

            int c=7, uc=0;
            while (c < (MAX_LINE_LENGTH-1))
            {
              if (linefeed[c] != ' ' &&
                  linefeed[c] != ']') // skip close bracket
              {
                name_unit[uc] = linefeed[c];
                uc++;
                c++;
              }
              else
                break; // get out
            }

            id = INI_UnitType(name_unit);
            if (id >= MAX_UNITTYPES) id--;
            
          } // found a new unit type
        }

        // New structure type
        if (section == INI_STRUCTURES)
        {
          // check if we found a new [STRUCTURE: part!
          if (strstr(linefeed,"[STRUCTURE:") != NULL)
          {
            // Get the name of the unit:
            // [STRUCTURE: <NAME>]
            // 123456789012345678]
            char name_structure[45];
            
            for (int nu=0; nu < 45; nu++)
              name_structure[nu]='\0';

            int c=12, uc=0;
            while (c < (MAX_LINE_LENGTH-1))
            {
              if (linefeed[c] != ' ' &&
                  linefeed[c] != ']') // skip close bracket
              {
                name_structure[uc] = linefeed[c];
                uc++;
                c++;
              }
              else
                break; // get out
            }

            id = INI_StructureType(name_structure);
            if (id >= MAX_STRUCTURETYPES) id--;
            
          } // found a new unit type
        }

        continue; // next line
      }

         // Check word only when in a section
      if (section != INI_GAME)
      {
        INI_Word(linefeed, lineword);
        wordtype = INI_WordType(lineword, section);

        /**** [UNITS] ****/
        // Valid ID
        if (section == INI_UNITS && id > -1)
        {          
          // Unit properties
          if (wordtype == WORD_HITPOINTS)     units[id].hp            = INI_WordValueINT(linefeed);
          
          if (wordtype == WORD_COST)          units[id].cost          = INI_WordValueINT(linefeed);

          if (wordtype == WORD_MOVESPEED)     units[id].speed         = INI_WordValueINT(linefeed);
          if (wordtype == WORD_TURNSPEED)     units[id].turnspeed     = INI_WordValueINT(linefeed);
          if (wordtype == WORD_ATTACKFREQ)    units[id].attack_frequency = INI_WordValueINT(linefeed);
          
          if (wordtype == WORD_SIGHT)         units[id].sight     = INI_WordValueINT(linefeed);              
          
          if (wordtype == WORD_RANGE)         units[id].range     = INI_WordValueINT(linefeed);
          if (wordtype == WORD_BUILDTIME)     units[id].build_time = INI_WordValueINT(linefeed);
          
          // Unit description
          if (wordtype == WORD_DESCRIPTION)
          {
            char n[256];
            INI_WordValueCHAR(linefeed, n);
            sprintf(units[id].name, "%s", n);
          }

          // Booleans
          if (wordtype == WORD_SECONDSHOT)             units[id].second_shot  = INI_WordValueBOOL(linefeed);          
          if (wordtype == WORD_ISINFANTRY)             units[id].infantry     = INI_WordValueBOOL(linefeed);
          if (wordtype == WORD_FREEROAM)               units[id].free_roam    = INI_WordValueBOOL(linefeed);
          if (wordtype == WORD_ISAIRBORN)              units[id].airborn      = INI_WordValueBOOL(linefeed);

          // Harvester specific properties.
          if (wordtype == WORD_HARVESTLIMIT)           units[id].credit_capacity   = INI_WordValueINT(linefeed);
          if (wordtype == WORD_HARVESTSPEED)           units[id].harvesting_speed  = INI_WordValueINT(linefeed);
          if (wordtype == WORD_HARVESTAMOUNT)          units[id].harvesting_amount = INI_WordValueINT(linefeed);

        }
      } 

      // Structure w0h00
      if (section == INI_STRUCTURES && id > -1)
      {
          if (wordtype == WORD_HITPOINTS)     structures[id].hp           = INI_WordValueINT(linefeed);
		  if (wordtype == WORD_FIXHP)         structures[id].fixhp        = INI_WordValueINT(linefeed);
          
          if (wordtype == WORD_POWERDRAIN)    structures[id].power_drain  = INI_WordValueINT(linefeed);
          if (wordtype == WORD_POWERGIVE)     structures[id].power_give   = INI_WordValueINT(linefeed);
          
          if (wordtype == WORD_COST)          structures[id].cost         = INI_WordValueINT(linefeed);
          if (wordtype == WORD_BUILDTIME)     structures[id].build_time   = INI_WordValueINT(linefeed);

      }
   
    }
 

    fclose(stream);  
  }
  
 
logbook("[GAME.INI] Done");  
}


void INI_LOAD_SKIRMISH(char filename[80], bool bScan)
{
    // search for new entry in previewed maps
    int iNew=-1;
    for (int i=0; i < 100; i++)
    {
        if (PreviewMap[i].name[0] == '\0')
        {
            iNew=i;
            break;
        }

    }

    if (iNew < 0)
        return;

	// Load file
    
	FILE *stream;					// file stream
	int section=INI_NONE;			// section
	int wordtype=WORD_NONE;			// word
    
	int iYLine=0;
	int iStart=0;

    if( (stream = fopen( filename, "r+t" )) != NULL )
    {
        char linefeed[MAX_LINE_LENGTH];
        char lineword[25];
        char linesection[30];

        for (int iCl=0; iCl < MAX_LINE_LENGTH; iCl++)
        {
            linefeed[iCl] = '\0';
            if (iCl < 25) lineword[iCl] = '\0';
            if (iCl < 30) linesection[iCl] = '\0';
        }

          // infinite loop baby
    while( !feof( stream ) )
    {
      INI_Sentence(stream, linefeed);

      // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (linefeed[0] == ';' ||
          linefeed[0] == '#' ||
         (linefeed[0] == '/' && linefeed[1] == '/') ||
          linefeed[0] == '\n' ||
          linefeed[0] == '\0')
          continue;   // Skip

      // Every line is checked for a new section.
      INI_Section(linefeed,linesection);

      if (linesection[0] != '\0' && strlen(linesection) > 1)
      {
		  int iOld=section;
        section= INI_SectionType(linesection, section);

        // section found
		if (iOld != section)
		{
			if (section == INI_MAP)
			{
				if (PreviewMap[iNew].terrain == NULL)
				{
					PreviewMap[iNew].terrain = create_bitmap(128,128);
					clear(PreviewMap[iNew].terrain);
					//clear_to_color(PreviewMap[iNew].terrain, makecol(255,255,255));
				}
				continue; // skip
			}
		}
      }

       // Okay, we found a new section; if its NOT [GAME] then we remember this one!
	
      if (section != INI_NONE)
      {
          INI_Word(linefeed, lineword);
          wordtype = INI_WordType(lineword, section);
      }
	  else
		  continue;

      if (section == INI_SKIRMISH)
      {
          if (wordtype == WORD_MAPNAME)
          {
            INI_WordValueSENTENCE(linefeed, PreviewMap[iNew].name);
            logbook(PreviewMap[iNew].name);
          }

		  if (wordtype == WORD_STARTCELL)
		  {
			  if (iStart == 0)
			  {
				for (int i=0; i < 5; i++)
					PreviewMap[iNew].iStartCell[i] = -1;
			  }

			  // start locations
			  if (iStart < 5)
			  {
				  PreviewMap[iNew].iStartCell[iStart]= INI_WordValueINT(linefeed);
				  iStart++;
			  }			  
		  }
      }

	  if (section == INI_MAP)
	  {			  
		  int iLength=strlen(linefeed);	  
		  
		  // END!
		  if (iLength < 2)
			  break; // done

		  for (int iX=0; iX < iLength; iX++)
		  {
			  char letter[1];
			  letter[0] = '\0';
			  letter[0] = linefeed[iX];
			  
              int iCll=iCellMake((iX+1),(iYLine+1));

              
			  int iColor=makecol(194, 125, 60);

			  // rock
			  if (letter[0] == '%') iColor = makecol(80,80,60);
			  if (letter[0] == '^') iColor = makecol(80,80,60);
			  if (letter[0] == '&') iColor = makecol(80,80,60);
			  if (letter[0] == '(') iColor = makecol(80,80,60);				

			  // mountain
			  if (letter[0] == 'R') iColor = makecol(48, 48, 36);
			  if (letter[0] == 'r') iColor = makecol(48, 48, 36);

			  // spicehill
			  if (letter[0] == '+') iColor = makecol(180,90,25); // bit darker

			  // spice
			  if (letter[0] == '-') iColor = makecol(186,93,32);

			  // HILLS (NEW)
			  if (letter[0] == 'H') iColor = makecol(188, 115, 50);
			  if (letter[0] == 'h') iColor = makecol(188, 115, 50);

              if (iCll > -1)
              {
                  if (iColor == makecol(194,125,60)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_SAND;
                  if (iColor == makecol(80,80,60)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_ROCK;
                  if (iColor == makecol(48,48,36)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_MOUNTAIN;
                  if (iColor == makecol(180,90,25)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_SPICEHILL;
                  if (iColor == makecol(186,93,32)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_SPICE;
                  if (iColor == makecol(188,115,50)) PreviewMap[iNew].mapdata[iCll]=TERRAIN_HILL;
              }



			  putpixel(PreviewMap[iNew].terrain, 1+(iX*2), 1+(iYLine*2), iColor);
			  putpixel(PreviewMap[iNew].terrain, 1+(iX*2)+1, 1+(iYLine*2), iColor);
			  putpixel(PreviewMap[iNew].terrain, 1+(iX*2)+1, 1+(iYLine*2)+1, iColor);
			  putpixel(PreviewMap[iNew].terrain, 1+(iX*2), 1+(iYLine*2)+1, iColor);

		  }
		  iYLine++;	
	  }

    } // end of file

	// starting points
	for (int i=0; i < 5; i++)
	{
		if (PreviewMap[iNew].iStartCell[i] > -1)
		{
			int x=iCellGiveX(PreviewMap[iNew].iStartCell[i]);
			int y=iCellGiveY(PreviewMap[iNew].iStartCell[i]);
            
			putpixel(PreviewMap[iNew].terrain, 1+(x*2), 1+(y*2), makecol(255,255,255));
			putpixel(PreviewMap[iNew].terrain, 1+(x*2)+1, 1+(y*2), makecol(255,255,255));
			putpixel(PreviewMap[iNew].terrain, 1+(x*2)+1, 1+(y*2)+1, makecol(255,255,255));
			putpixel(PreviewMap[iNew].terrain, 1+(x*2), 1+(y*2)+1, makecol(255,255,255));
		}
	}
	fclose(stream);

	} // file exists

}

/*
Pre-scanning of skirmish maps:

- open file
- read [SKIRMISH] data (name of map, startcells, etc)
- create preview of map in BITMAP (minimap preview)
*/
void INI_PRESCAN_SKIRMISH()
{
   // scans for all ini files                                       
   INIT_PREVIEWS(); // clear all of them

   char filename[80];
   memset(filename, 0, sizeof(filename));
   
   for (int i=0; i < MAX_SKIRMISHMAPS; i++)
   {
       if (i < 10)
           sprintf(filename, "skirmish//map0%d.ini", i);
       else
           sprintf(filename, "skirmish//map%d.ini",i);

       INI_LOAD_SKIRMISH(filename, true);        
   }
   
}

