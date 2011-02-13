/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "include/d2tmh.h"

using namespace std;

/*
 Read a line in the INI file and put it into currentLine
*/
inline bool caseInsCharCompareN(char a, char b) {
   return(toupper(a) == toupper(b));
}

bool caseInsCompare(const string& s1, const string& s2) {
   return((s1.size( ) == s2.size( )) &&
          equal(s1.begin( ), s1.end( ), s2.begin( ), caseInsCharCompareN));
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

/*********************************************************************************/
// Reads out INPUT , will check for a [ at [0] and then checks till ], it will fill section[]
// with the chars in between. So : [MAP] -> section = 'MAP'. Use function INI_SectionType(..)
// to get the correct ID for that.
void INI_Section(char input[MAX_LINE_LENGTH], char section[30]) {

	int pos=0;
	int end_pos=-1;

	memset(section, '\0', sizeof(section));

	// check if the first character is a '['
	if (input[0] == '[') {
		pos=1; // Begin at character 1

		// find the ending ]
		while (pos < (MAX_LINE_LENGTH-1)) {
			if (input[pos] == ']') {
			  end_pos=pos-1;
			  break;
			}
			pos++;
		}

		if (end_pos > 1 && end_pos < 29) {
			for (int wc=0; wc < end_pos; wc++) {
				section[wc]=input[wc+1];
			}
			section[end_pos]='\0'; // terminate string
		}
	}
}

// Reads out INPUT and will check for an '=' Everything at the left of the
// '=' IS a word and will be put in 'word[]'. Use function INI_WordType(char word[25]) to get
// the correct ID tag.
void INI_Word(char input[MAX_LINE_LENGTH], char word[25]) {
	int word_pos = INI_GetPositionOfCharacter(input, '=');

	memset(word, '\0', sizeof(word));

	if (word_pos > -1 && word_pos < 23) {
		for (int wc=0; wc < word_pos; wc++) {
			word[wc]=input[wc];
		}

		word[word_pos]='\0'; // terminate string
	}
}

/**
 * Return true when string "toFind" is in source string. Else return false.
 *
 * @param source
 * @param toFind
 * @return
 */
bool isInString(string source, string toFind) {
	 string::size_type loc = source.find( toFind , 0 );
	 if( loc == 0 ) {
	   return true;
	 }
	 return false; // not found in string
}

string INI_SceneFileToScene(string scenefile) {
	// wsa / data
	if (isInString(scenefile, "HARVEST.WSA"))		return "harvest";
	if (isInString(scenefile, "IX.WSA"))		return "ix";
	if (isInString(scenefile, "SARDUKAR.WSA"))		return "sardukar";
	if (isInString(scenefile, "PALACE.WSA"))		return "palace";
	if (isInString(scenefile, "REPAIR.WSA"))		return "repair";
	if (isInString(scenefile, "HVYFTRY.WSA"))		return "hvyftry";
	if (isInString(scenefile, "HEADQRTS.WSA"))		return "headqrts";
	if (isInString(scenefile, "QUAD.WSA"))		return "quad";
	if (isInString(scenefile, "LTANK.WSA"))		return "ltank";

	char msg[255];
	sprintf(msg, "Failed to map dune 2 scenefile [%s] to a d2tm scene file", scenefile.c_str());
	logbook(msg);

	return "unknown";
}

int INI_StructureType(string structureName) {

	if (isInString(structureName, "WINDTRAP"))		return WINDTRAP;
	if (isInString(structureName, "PALACE"))		return PALACE;
	if (isInString(structureName, "HEAVYFACTORY"))		return HEAVYFACTORY;
	if (isInString(structureName, "LIGHTFACTORY"))		return LIGHTFACTORY;
	if (isInString(structureName, "CONSTYARD"))		return CONSTYARD;
	if (isInString(structureName, "SILO"))		return SILO;
	if (isInString(structureName, "HIGHTECH"))		return HIGHTECH;
	if (isInString(structureName, "IX"))		return IX;
	if (isInString(structureName, "REPAIR"))		return REPAIR;
	if (isInString(structureName, "RADAR"))		return RADAR;
	if (isInString(structureName, "REFINERY"))		return REFINERY;
	if (isInString(structureName, "WOR"))		return WOR;
	if (isInString(structureName, "BARRACKS"))		return BARRACKS;
	if (isInString(structureName, "STARPORT"))		return STARPORT;
	if (isInString(structureName, "TURRET"))		return TURRET;
	if (isInString(structureName, "ROCKETTURRET"))		return RTURRET;
	if (isInString(structureName, "STARPORT"))		return STARPORT;
	if (isInString(structureName, "SLAB"))		return SLAB1;
	if (isInString(structureName, "4SLAB"))		return SLAB4;
	if (isInString(structureName, "WALL"))		return WALL;

	assert(false); // no recognition is fail.

	return 0; // just in case some miracle happened, we need to go on and not crash everything.
}

// Reads out word[], checks structure type, and returns actual source-id
int INI_StructureType(char word[256]) {
	string wordAsString(word);
	return INI_StructureType(wordAsString);
}

// Reads out word[], does a string compare and returns type id
int INI_WordType(char word[25], int section)
{

//	char msg[255];
//	memset(msg, 0, sizeof(msg));
//	sprintf(msg, "Going to find word-type for [%s]", word);
//	logbook(msg);

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

    // what structure type produces this kind of unit?
    if (strcmp(word, "Producer") == 0)				return WORD_PRODUCER;

    // HARVESTER
    if (strcmp(word, "MaxCredits") == 0)			return WORD_HARVESTLIMIT;
    if (strcmp(word, "HarvestSpeed") == 0)			return WORD_HARVESTSPEED;
    if (strcmp(word, "HarvestAmount") == 0)			return WORD_HARVESTAMOUNT;
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
  else if (section == INI_SETTINGS)
  {
	  if (strcmp(word, "FullScreen") == 0)   return WORD_FULLSCREEN;
	  if (strcmp(word, "ScreenWidth") == 0)  return WORD_SCREENWIDTH;
	  if (strcmp(word, "ScreenHeight") == 0) return WORD_SCREENHEIGHT;
	  if (strcmp(word, "MP3Music") == 0)	 return WORD_MP3MUSIC;
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

 if (strcmp(section, "SETTINGS") == 0)
    return INI_SETTINGS;

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

int INI_GetPositionOfCharacter(char result[MAX_LINE_LENGTH], char c) {
	string resultString(result);
	return resultString.find_first_of(c, 0);
}


/**
 * Return the part after the = sign as string.
 *
 * @param result
 * @return
 */
string INI_WordValueString(char result[MAX_LINE_LENGTH]) {
	string resultAsString(result);
	int isPos = INI_GetPositionOfCharacter(result, '=');
	int length = resultAsString.size();
	return resultAsString.substr(isPos+1);
}


// Reads out 'result' and will return the value after the '='. Returns nothing but will put
// the result in 'value[25]'. Max argument may be 256 characters!
void INI_WordValueCHAR(char result[MAX_LINE_LENGTH], char value[256]) {
	int pos=0;
	int is_pos=-1;

	// clear out entire string
	memset(value, 0, sizeof(value));
	for (int i = 0; i < 256; i++) {
		value[i] = '\0';
	}

	while (pos < (MAX_LINE_LENGTH-1))	{
		if (result[pos] == '=')	{
			is_pos=pos;
			break;
		}
		pos++;
	}

	if (is_pos > -1) {
		// Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
		// space.
		int end_pos=-1;

		while (pos < (MAX_LINE_LENGTH-1)) {
			if (result[pos] == '\0' || result[pos] == '\n') {
				end_pos=(pos-1);
				break;
			}
			pos++;
		}

		// End position found!
		if (end_pos > -1) {
			// We know the END position. We will use that piece of string to read out a number.

			// Copy the part to 'value', Make sure we won't get outside the array of the character.
			int cp=is_pos+1;
			int c=0;
			while (cp <= end_pos) {
			  value[c] = result[cp];
			  c++;
			  cp++;
			  if (c > 80) {
				  break;
			  }
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
  return caseInsCompare(val, "true");
}
// return ID of structure
int getStructureTypeFromChar(char *structure)
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

/**
 * Create seed map.
 *
 * @param seed
 */
void INI_Load_seed(int seed) {

	char msg[256];
	sprintf(msg, "Generating seed map with seed %d.", seed);
	logbook(msg);

	cSeedMapGenerator *seedGenerator = new cSeedMapGenerator(seed);

	cSeedMap *seedMap = seedGenerator->generateSeedMap();
	logbook("Seedmap generated");

	for (int mapY = 0; mapY < 64; mapY++) {
		for (int mapX = 0; mapX < 64; mapX++) {
			char c = seedMap->getCellTypeCharacter(mapX, mapY);
			int type = seedMap->getCellType(mapX, mapY);
			int iCell = iCellMake(mapX, mapY);
			mapEditor.createCell(iCell, type, 0);
		}
	}
	logbook("Seedmap converted into D2TM map.");
}


// Load
/**
 * Return true when line starts with:
 * ; # // \n \0
 *
 * @param linefeed
 * @return
 */
bool isCommentLine(char linefeed[MAX_LINE_LENGTH]) {
    return linefeed[0] == ';' || linefeed[0] == '#' || (linefeed[0] == '/' && linefeed[1] == '/') || linefeed[0] == '\n' || linefeed[0] == '\0';
}

/**
 * Return the name of the directory to look for by house id.
 * (ie iHouse == ATREIDES) returns "atreides"
 * @param iHouse
 * @return
 */
string INI_GetHouseDirectoryName(int iHouse) {
	if (iHouse == ATREIDES)		return "atreides";
	if (iHouse == HARKONNEN)	return "harkonnen";
	if (iHouse == SARDAUKAR)	return "sardaukar";
	if (iHouse == ORDOS)		return "ordos";
	if (iHouse == FREMEN)		return "fremen";
	if (iHouse == MERCENARY)	return "mercenary";
	return "unknown";
}

void INI_Load_Regionfile(int iHouse, int iMission) {
	char filename[256];
    memset(filename, 0, sizeof(filename));
	sprintf(filename, "campaign/%s/mission%d.ini", INI_GetHouseDirectoryName(iHouse).c_str(), iMission);

	cLogger::getInstance()->log(LOG_INFO, COMP_REGIONINI, "Opening mission file", filename);

	////////////////////////////
	// START OPENING FILE
	////////////////////////////
	FILE *stream;					// file stream
	int wordtype=WORD_NONE;			// word
	int iRegionIndex=-1;
    int iRegionNumber=-1;
    int iRegionConquer=-1;

    // open file
    if( (stream = fopen( filename, "r+t" )) != NULL )  {

		char linefeed[MAX_LINE_LENGTH];
		char lineword[25];
		char linesection[30];

		memset(lineword, '\0', sizeof(lineword));
		memset(linesection, '\0', sizeof(linesection));

		while( !feof( stream ) ) {
			INI_Sentence(stream, linefeed);

			// Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
			// character (which is "//", ";" or "#"), or an empty line, then skip it
			if (isCommentLine(linefeed)) {
			  continue;   // Skip
			}

			wordtype = WORD_NONE;

			// Every line is checked for a new section.
			INI_Word(linefeed, lineword);
			wordtype = INI_WordType(lineword, SEC_REGION);

			if (wordtype == WORD_REGION) {
			  iRegionNumber=-1;
			  iRegionConquer=-1;
			  iRegionNumber = INI_WordValueINT(linefeed)-1;
			} else if (wordtype == WORD_REGIONCONQUER) {
			  iRegionNumber=-1;
			  iRegionConquer=-1;
			  iRegionIndex++;
			  iRegionConquer = INI_WordValueINT(linefeed)-1;
			  game.iRegionConquer[iRegionIndex] = iRegionConquer;
			}

			if (iRegionIndex > -1 || iRegionNumber > -1) {
				if (wordtype == WORD_REGIONHOUSE) {
					char cHouseRegion[256];
					memset(cHouseRegion, 0, sizeof(cHouseRegion));
					INI_WordValueCHAR(linefeed, cHouseRegion);

					logbook("Region house");
					int iH= getHouseFromChar(cHouseRegion);

					if (iRegionNumber > -1) {
						world[iRegionNumber].iHouse = iH;
						world[iRegionNumber].iAlpha = 255;
					}

					if (iRegionConquer > -1) {
						game.iRegionHouse[iRegionIndex] = iH;
					}

				}

				if (wordtype == WORD_REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1) {
					char cHouseText[256];
					INI_WordValueSENTENCE(linefeed, cHouseText);
					sprintf(game.cRegionText[iRegionIndex], "%s", cHouseText);
				}

				if (wordtype == WORD_REGIONSELECT) {
					if (iRegionNumber > -1) {
						world[iRegionNumber].bSelectable = INI_WordValueBOOL(linefeed);
					}
				}

			}
		} // while

	   fclose(stream);
	   logbook("[CAMPAIGN] Done");
	   return;
	}

	logbook("[CAMPAIGN] Error, could not open file"); // make note on logbook
}

// SCENxxxx.ini loader (for both DUNE II as for DUNE II - The Maker)
int getUnitTypeFromChar(char chunk[35]) {
	string unitString(chunk);
    if (caseInsCompare(unitString, "Harvester"))	return HARVESTER;
    if (caseInsCompare(unitString, "Tank"))			return TANK;
    if (caseInsCompare(unitString, "COMBATTANK"))	return TANK;
    if (caseInsCompare(unitString, "Siege Tank"))   return SIEGETANK;
    if (caseInsCompare(unitString, "SIEGETANK"))   	return SIEGETANK;
    if (caseInsCompare(unitString, "Launcher"))		return LAUNCHER;
    if (caseInsCompare(unitString, "Trooper"))		return TROOPER;
    if (caseInsCompare(unitString, "Troopers"))		return TROOPERS;
    if (caseInsCompare(unitString, "Sonic Tank"))	return SONICTANK;
    if (caseInsCompare(unitString, "SONICTANK"))	return SONICTANK;
    if (caseInsCompare(unitString, "Quad"))			return QUAD;
    if (caseInsCompare(unitString, "Trike"))		return TRIKE;
    if (caseInsCompare(unitString, "Raider Trike")) return RAIDER;
    if (caseInsCompare(unitString, "RAIDER")) return RAIDER;
    if (caseInsCompare(unitString, "Soldier"))		return SOLDIER;
    if (caseInsCompare(unitString, "Infantry"))		return INFANTRY;
    if (caseInsCompare(unitString, "Devastator"))   return DEVASTATOR;
    if (caseInsCompare(unitString, "Deviator"))		return DEVIATOR;
	if (caseInsCompare(unitString, "MCV"))			return MCV;
	if (caseInsCompare(unitString, "Trike"))		return TRIKE;
	if (caseInsCompare(unitString, "Soldier"))		return SOLDIER;
	if (caseInsCompare(unitString, "CarryAll"))		return CARRYALL;
	if (caseInsCompare(unitString, "Ornithopter"))	return ORNITHOPTER;
	if (caseInsCompare(unitString, "Sandworm"))		return SANDWORM;
	if (caseInsCompare(unitString, "Saboteur"))		return SABOTEUR;
	if (caseInsCompare(unitString, "MISSILE"))		return MISSILE;
	if (caseInsCompare(unitString, "ONEFREMEN"))		return UNIT_FREMEN_ONE;
	if (caseInsCompare(unitString, "THREEFREMEN"))		return UNIT_FREMEN_THREE;

	char msg[255];
	sprintf(msg, "getUnitTypeFromChar could not determine what unit type '%s' (original is '%s%') is. Returning -1; this will probably cause problems.", unitString.c_str(), chunk);
	logbook(msg);
	return -1;
}

int getHouseFromChar(char chunk[25])
{
    if (strcmp(chunk, "Atreides") == 0)    return ATREIDES;
    if (strcmp(chunk, "Harkonnen") == 0)   return HARKONNEN;
    if (strcmp(chunk, "Ordos") == 0)       return ORDOS;
    if (strcmp(chunk, "Sardaukar") == 0)   return SARDAUKAR;
    if (strcmp(chunk, "Mercenary") == 0)   return MERCENARY;
    if (strcmp(chunk, "Fremen") == 0)      return FREMEN;
    if (strcmp(chunk, "Corrino") == 0)     return CORRINO;
    if (strcmp(chunk, "General") == 0)     return GENERALHOUSE;
    char msg[255];
    sprintf(msg, "getHouseFromChar could not determine what house type '%s' is. Returning -1; this will probably cause problems.", chunk);
    logbook(msg);
    return -1;
}

/**
 * Taken the region conquered by player, in sequential form (meaning, the 1st region the
 * player conquers, corresponds with techlevel 1. While the 8th, 9th or 10th region correspond
 * with techlevel 4.
 *
 * This assumes that the player conquers the world in a fixed set of regions.
 *
 * TODO: Make this moddable.
 *
 * @param iRegion
 * @return
 */
int getTechLevelByRegion(int iRegion) {
    if(iRegion == 1) return 1;
    if(iRegion == 2 || iRegion == 3 || iRegion == 4) return 2;
    if(iRegion == 5 || iRegion == 6 || iRegion == 7) return 3;
    if(iRegion == 8 || iRegion == 9 || iRegion == 10) return 4;
    if(iRegion == 11 || iRegion == 12 || iRegion == 13) return 5;
    if(iRegion == 14 || iRegion == 15 || iRegion == 16) return 6;
    if(iRegion == 17 || iRegion == 18 || iRegion == 19) return 7;
    if(iRegion == 20 || iRegion == 21) return 8;
    return 9;
}

/**
 * Returns a string, containing the relative path to the scenario file for
 * the given house and region id.
 * @param iHouse
 * @param iRegion
 * @return
 */
string INI_GetScenarioFileName(int iHouse, int iRegion) {
	string cHouse;

	// each house has a letter for the scenario file
	if (iHouse == ATREIDES) 	cHouse = "a";
	if (iHouse == HARKONNEN) 	cHouse = "h";
	if (iHouse == ORDOS) 		cHouse = "o";
	if (iHouse == SARDAUKAR)	cHouse = "s";
	if (iHouse == MERCENARY)	cHouse = "m";
	if (iHouse == FREMEN)		cHouse = "f";

	char filename[256];
	memset(filename, '\0', sizeof(filename));

	if(iRegion < 10) {
		sprintf(filename, "campaign/maps/scen%s00%d.ini", cHouse.c_str(), iRegion);
	} else {
		sprintf(filename, "campaign/maps/scen%s0%d.ini", cHouse.c_str(), iRegion);
	}

	return string(filename);
}


void INI_Load_scenario(int iHouse, int iRegion) {
    game.bSkirmish = false;
    game.mission_init();

    string filename = INI_GetScenarioFileName(iHouse, iRegion);

    game.iMission = getTechLevelByRegion(iRegion);

    char msg[256];

    sprintf(msg, "[SCENARIO] '%s' (Mission %d)", filename.c_str(), game.iMission);
    logbook(msg);
    logbook("[SCENARIO] Opening file");

    // declare some temp fields while reading the scenario file.
    int blooms[30], fields[30];
    char value[256];
    memset(blooms, -1, sizeof (blooms));
    memset(fields, -1, sizeof (fields));

    FILE *stream;
    int section=INI_NONE;
    int wordtype=WORD_NONE;
    int iPlayerID = -1;
    int iHumanID = -1;
    bool bSetUpPlayers = true;
    int iPl_credits[MAX_PLAYERS];
    int iPl_house[MAX_PLAYERS];
    int iPl_quota[MAX_PLAYERS];

    memset(iPl_credits, 0, sizeof (iPl_credits));
    memset(iPl_house, -1, sizeof (iPl_house));
    memset(iPl_quota, 0, sizeof (iPl_quota));

    if( (stream = fopen( filename.c_str(), "r+t" )) != NULL ) {
		char linefeed[MAX_LINE_LENGTH];
		char lineword[25];
		char linesection[30];

		memset(lineword, '\0', sizeof(lineword));
		memset(linesection, '\0', sizeof(linesection));
		memset(linefeed, '\0', sizeof(linefeed));

		// infinite loop baby
		while( !feof( stream ) ) {
			INI_Sentence(stream, linefeed);

			// Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
			// character (which is "//", ";" or "#"), or an empty line, then skip it
			if (isCommentLine(linefeed))   continue;   // Skip

			// Every line is checked for a new section.
			INI_Section(linefeed, linesection);

			// line is not starting empty and section is found
			if (linesection[0] != '\0' && strlen(linesection) > 1) {
				section = SCEN_INI_SectionType(linesection, section);
				char msg[255];
				sprintf(msg, "[SCENARIO] found section '%s', resulting in section id [%d]", linesection, section);
				logbook(msg);

				if (section >= INI_HOUSEATREIDES &&	section <= INI_HOUSEMERCENARY) {
					iPlayerID++;

					if (iPlayerID > (MAX_PLAYERS-1)) {
						iPlayerID = (MAX_PLAYERS-1);
					}

					if (section == INI_HOUSEATREIDES)   iPl_house[iPlayerID] = ATREIDES;
					if (section == INI_HOUSEORDOS)      iPl_house[iPlayerID] = ORDOS;
					if (section == INI_HOUSEHARKONNEN)  iPl_house[iPlayerID] = HARKONNEN;
					if (section == INI_HOUSEMERCENARY)  iPl_house[iPlayerID] = MERCENARY;
					if (section == INI_HOUSEFREMEN)     iPl_house[iPlayerID] = FREMEN;
					if (section == INI_HOUSESARDAUKAR)  iPl_house[iPlayerID] = SARDAUKAR;

					char msg[255];
					sprintf(msg, "[SCENARIO] Setting house to [%d] for playerId [%d]", iPl_house[iPlayerID], iPlayerID);
					logbook(msg);
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

					string scenefile = INI_WordValueString(linefeed);
					string scene = INI_SceneFileToScene(scenefile);

					scene = INI_SceneFileToScene(scenefile);

					if (!isInString(scene, "unknown")) {
						LOAD_SCENE(scene);
					}
				}

				if (wordtype == WORD_FOCUS)
				{
					player[0].focus_cell = INI_WordValueINT(linefeed);
					mapCamera->centerAndJumpViewPortToCell(player[0].focus_cell);
				}
			}

			// Dune 2 house found, load player data
			if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY)
			{
				char msg[255];
				memset(msg, 0, sizeof(msg));
				sprintf(msg, "Section is between atreides and mercenary, the playerId is [%d]. WordType is [%d]", iPlayerID, wordtype);
				logbook(msg);
				// link house (found, because > -1)
				if (iPlayerID > -1) {
					if (wordtype == WORD_BRAIN)
					{
						char cBrain[256];
						memset(cBrain, 0, sizeof(cBrain));
						INI_WordValueCHAR(linefeed, cBrain);

						char msg[255];
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "Brain is [%s]", cBrain);
						logbook(msg);

						// We know the human brain now, this should be player 0...
						if (strcmp(cBrain, "Human") == 0) {
							char msg[255];
							memset(msg, 0, sizeof(msg));
							sprintf(msg, "Found human player for id [%d]", iPlayerID);
							logbook(msg);
							iHumanID = iPlayerID;
						} else {
							logbook("This brain is not human...");
						}

					} else if (wordtype == WORD_CREDITS) {
						int credits = INI_WordValueINT(linefeed)-1;
						char msg[255];
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "Set credits for player id [%d] to [%d]", iPlayerID, credits);
						logbook(msg);

						iPl_credits[iPlayerID] = credits;
					} else if (wordtype == WORD_QUOTA) {
						iPl_quota[iPlayerID] = INI_WordValueINT(linefeed);
					}
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
				logbook("Going to setup players");
                int iRealID=1;

                for (int iP=0; iP < MAX_PLAYERS; iP++) // till 6 , since player 6 itself is sandworm
                {
                	char msg[255];
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "House for id [%d] is [%d] - human id is [%d]", iP, iPl_house[iP], iHumanID);
					logbook(msg);
                    if (iPl_house[iP] > -1) {
                        if (iP == iHumanID)
                        {
                        	char msg[255];
							memset(msg, 0, sizeof(msg));
							sprintf(msg, "Setting up human player, credits to [%d]", iPl_credits[iP]);
							logbook(msg);
							player[HUMAN].credits = iPl_credits[iP];
							player[HUMAN].setHouse(iPl_house[iP]);
                            player[HUMAN].iTeam = 0;
                            game.iHouse = iPl_house[iP];
                            assert(drawManager);
                            if (drawManager->getCreditsDrawer())  {
                            	drawManager->getCreditsDrawer()->setCredits();
                            }

                            if (iPl_quota[iP] > 0) {
                                game.iWinQuota = iPl_quota[iP];
                            }
						}
						else
						{
                            // CPU player
                            player[iRealID].iTeam = 1; // All AI players are on the same team

                            // belong to player team
                            if (iPl_house[iP] == FREMEN) {
                                if (player[HUMAN].getHouse() == ATREIDES) {
                                    player[iRealID].iTeam = 0;
                                }
                            }

							player[iRealID].credits = iPl_credits[iP];
							player[iRealID].setHouse(iPl_house[iP]);
							iRealID++;
						}
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
							int iHouse = getHouseFromChar(chunk);

							// Search for a player with this house
							for (int i=0; i < MAX_PLAYERS; i++)
								if (player[i].getHouse() == iHouse)
								{
									iController = i; // set controller here.. phew
									break;
								}

								// HACK HACK : Set up fremen house here
							if (iHouse == FREMEN)
							{
								player[5].setHouse(FREMEN); // set up palette
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
							iType = getUnitTypeFromChar(chunk);
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
							player[0].setHouse(iPl_house[iP]);
                            player[0].iTeam=0;
                            game.iHouse = iPl_house[iP];

                            if (iPl_quota[iP] > 0) {
                                game.iWinQuota = iPl_quota[iP];
                            }
						}
						else
						{
                            // CPU player
                            player[iRealID].iTeam = 1; // All AI players are on the same team

                            // belong to player team
                            if (iPl_house[iP] == FREMEN) {
                                if (player[HUMAN].getHouse() == ATREIDES) {
                                    player[iRealID].iTeam = 0;
                                }
                            }

							player[iRealID].credits = iPl_credits[iP];
							player[iRealID].setHouse(iPl_house[iP]);
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
					for (int ic=0; ic < 25; ic++) {
						chunk[ic] = '\0';
					}

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

					// this line is not GENXXX
						if (bGen == false)
						{
							if (iPart == 0)
							{
								int iHouse = getHouseFromChar(chunk);

								// Search for a player with this house
								for (int i=0; i < MAX_PLAYERS; i++)
								{
									//char msg[80];
									//sprintf(msg, "i=%d, ihouse=%d, house=%d", i, iHouse, player[i].house);
									//logbook(msg);
									if (player[i].getHouse() == iHouse)
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
								iType = getStructureTypeFromChar(chunk);
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
								int iHouse= getHouseFromChar(chunk);;

								// Search for a player with this house
								for (int i=0; i < MAX_PLAYERS; i++)
								{
									if (player[i].getHouse() == iHouse)
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
								for (int cc=3; cc < iIS; cc++)
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
				// anything lower than SLAB1 means a 'normal' structure (TODO: make this less tight coupled)
				if (iType < SLAB1) {
					if (iType != CONSTYARD) {
						cStructureFactory::getInstance()->createSlabForStructureType(iCell, iType);
					}
					cStructureFactory::getInstance()->createStructure(iCell, iType, iController, 100);
				} else {
					if (iType == SLAB1)
                    {
                        mapEditor.createCell(iCell, TERRAIN_SLAB, 0);
                        //map.cell[iCell].tile = SLAB;
                    }
					if (iType == WALL)
					{
						mapEditor.createCell(iCell, TERRAIN_WALL, 0);
					}
				}
			}
			else {
				logbook("WARNING: Identifying house/controller of structure (typo?)");
			}
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
                        int iHouse = getHouseFromChar(chunk);

						if (iHouse > -1)
						{
							// Search for a player with this house
							for (int i=0; i < MAX_PLAYERS; i++)
							{
								if (player[i].getHouse() == iHouse)
								{
									iController = i; // set controller here.. phew
									break;
								}
							}
						}
					}
                    else if (iPart == 1)
                    {
                        iType = getUnitTypeFromChar(chunk);

                    }
                    else if (iPart == 2)
                    {
                        // Homebase is home of that house
                        if (strcmp(chunk, "Homebase") == 0) {
                            iCell = player[iController].focus_cell;
                        }
                        else
                        {
                            // enemy base

							if (iController == 0)
							{
                            // Find corresponding house and get controller
                            for (int i=0; i < MAX_PLAYERS; i++)
                                if (player[i].getHouse() == iHouse && i != iController)
                                {
                                    iCell = player[i].focus_cell;
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
			if (player[iP].max_credits < 1000) {
				player[iP].max_credits = 1000;
			}


	}

    mapEditor.smoothMap();

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

			  mapEditor.createCell(blooms[iB], TERRAIN_BLOOM, 0);

          }
      }

     // At this point, show list of unit types

	// now add the fields
    for (int iB=0; iB < 30; iB++)
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
		  	  mapEditor.createField(fields[iB], TERRAIN_SPICE, 25+(rnd(50)));
		  }

	}

    logbook("[SCENARIO] Done reading");
  }
    player[AI_WORM].iTeam=-2;
    mapEditor.smoothMap();
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

    sprintf(path, "campaign/briefings/%s", filename);

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
      if (isCommentLine(linefeed))  continue;   // Skip

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


    	fclose(stream);
    }

    logbook("[BRIEFING] File opened");

}


// Game.ini loader
void INI_Install_Game(string filename) {
  logbook("[GAME.INI] Opening file");

  FILE *stream;
  int section=INI_GAME;
  int wordtype=WORD_NONE;
  int id=-1;

  int side_r, side_b, side_g;
  int team_r, team_g, team_b;   // rgb colors for a team
  side_r=side_g=side_b=-1;
  team_r=team_g=team_b=-1;

  char msg[255];
  sprintf(msg, "Opening game settings from : %s", filename.c_str());
  logbook(msg);

  if( (stream = fopen( filename.c_str(), "r+t" )) != NULL ) {
    char linefeed[MAX_LINE_LENGTH];
    char lineword[25];
    char linesection[30];

    // infinite loop baby
    while( !feof( stream ) )
    {
      INI_Sentence(stream, linefeed);

      // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
      // character (which is "//", ";" or "#"), or an empty line, then skip it
      if (isCommentLine(linefeed))
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
          if (section == INI_SETTINGS)    logbook("[GAME.INI] -> [SETTINGS]");
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
        if (section == INI_UNITS) {
          // check if we found a new [UNIT part!
          if (strstr(linefeed,"[UNIT:") != NULL) {
            // Get the name of the unit:
            // [UNIT: <NAME>]
            // 1234567890123...]
            char name_unit[35];

            for (int nu=0; nu < 35; nu++)
              name_unit[nu]='\0';

            int c=7, uc=0;
            while (c < (MAX_LINE_LENGTH-1)) {
              if (linefeed[c] != ' ' &&
                  linefeed[c] != ']') // skip close bracket
              {
                name_unit[uc] = linefeed[c];
                uc++;
                c++;
              } else {
                break; // get out
              }
            }

            id = getUnitTypeFromChar(name_unit);
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
      if (section != INI_GAME) {
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

          if (wordtype == WORD_PRODUCER) {
        	  string producerString = INI_WordValueString(linefeed);
        	  // determine structure type from that
        	  int type = INI_StructureType(producerString);
//        	  int type = INI_StructureType(producerString.c_str());
        	  units[id].structureTypeItLeavesFrom = type;
          }
        }
      }

      // Structure w0h00
      if (section == INI_STRUCTURES && id > -1)
      {
          if (wordtype == WORD_HITPOINTS)     {
        	  structures[id].hp           = INI_WordValueINT(linefeed);
          }
		  if (wordtype == WORD_FIXHP)         structures[id].fixhp        = INI_WordValueINT(linefeed);

          if (wordtype == WORD_POWERDRAIN)    structures[id].power_drain  = INI_WordValueINT(linefeed);
          if (wordtype == WORD_POWERGIVE)     structures[id].power_give   = INI_WordValueINT(linefeed);

          if (wordtype == WORD_COST)          structures[id].cost         = INI_WordValueINT(linefeed);
          if (wordtype == WORD_BUILDTIME)     structures[id].build_time   = INI_WordValueINT(linefeed);

      }

      if (section == INI_SETTINGS) {
		  switch (wordtype) {
			  case WORD_FULLSCREEN:
				  game.windowed = (INI_WordValueBOOL(linefeed) == false);
				  break;
			  case WORD_SCREENWIDTH:
				  game.ini_screen_width=INI_WordValueINT(linefeed);
				  break;
			  case WORD_SCREENHEIGHT:
				  game.ini_screen_height=INI_WordValueINT(linefeed);
				  break;
			  case WORD_MP3MUSIC:
				  game.bMp3 = INI_WordValueBOOL(linefeed);
				  break;
		  }
	  }
    } // while

    fclose(stream);
  } // if

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

    if (iNew < 0) {
        return;
    }


    // first clear it all out
	for (int x=0; x < game.map_width; x++) {
	  for (int y = 0; y < game.map_height; y++) {
		  int cll = iCellMake(x, y);
		  PreviewMap[iNew].mapdata[cll]=TERRAIN_SAND;
	  }
}

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
      if (isCommentLine(linefeed)) {
          continue;   // Skip
      }

      // Every line is checked for a new section.
      INI_Section(linefeed,linesection);

      if (linesection[0] != '\0' && strlen(linesection) > 1) {
    	  int iOld=section;
    	  section= INI_SectionType(linesection, section);

			// section found
			if (iOld != section) {
				if (section == INI_MAP) {
					if (PreviewMap[iNew].terrain == NULL) {
						PreviewMap[iNew].terrain = create_bitmap(128,128);
						clear(PreviewMap[iNew].terrain);
						//clear_to_color(PreviewMap[iNew].terrain, makecol(255,255,255));
					}
					continue; // skip
				}
			}
      }

       // Okay, we found a new section; if its NOT [GAME] then we remember this one!

      if (section != INI_NONE) {
          INI_Word(linefeed, lineword);
          wordtype = INI_WordType(lineword, section);
      } else {
		  continue;
      }

      if (section == INI_SKIRMISH)
      {
          if (wordtype == WORD_MAPNAME)
          {
            INI_WordValueSENTENCE(linefeed, PreviewMap[iNew].name);
            //logbook(PreviewMap[iNew].name);
          }

		  if (wordtype == WORD_STARTCELL) {
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
		  if (iLength < 2) {
			  break; // done
		  }

		  for (int iX=0; iX < iLength; iX++) {
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

              if (iCll > -1) {
                  if (iColor == makecol(194,125,60)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_SAND;
                  } else if (iColor == makecol(80,80,60)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_ROCK;
                  } else if (iColor == makecol(48,48,36)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_MOUNTAIN;
                  } else if (iColor == makecol(180,90,25)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_SPICEHILL;
                  } else if (iColor == makecol(186,93,32)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_SPICE;
                  } else if (iColor == makecol(188,115,50)) {
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_HILL;
                  } else {
                	  char msg[255];
                	  sprintf(msg, "iniLoader::skirmish() - Could not determine terrain type for char \"%c\", falling back to SAND", letter[0]);
                	  logbook(msg);
                	  PreviewMap[iNew].mapdata[iCll]=TERRAIN_SAND;
                	  iColor = makecol(255, 255, 255);
                  }
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
void INI_PRESCAN_SKIRMISH() {
   // scans for all ini files
   INIT_PREVIEWS(); // clear all of them

   	struct al_ffblk file;
	if (!al_findfirst("skirmish/*", &file, FA_ARCH)) {
		do {
			char msg[1024];
			char fullname[1024];
			sprintf(fullname, "skirmish/%s", file.name);
			sprintf(msg, "Loading skirmish map: %s", fullname);
			logbook(msg);
			INI_LOAD_SKIRMISH(fullname, true);
	  } while (!al_findnext(&file));
	} else {
		logbook("No skirmish maps found in skirmish directory.");
	}
	al_findclose(&file);

}

// this code should make it possible to read any ini file in the skirmish
// directory. However, Allegro 4.2.0 somehow gives weird results.
// when upgrading to Allegro 4.2.2, the method works. But FBLEND crashes, even
// after recompiling it against Allegro 4.2.2...
//
// See: http://www.allegro.cc/forums/thread/600998
