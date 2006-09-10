/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"
#include "d2tmc.h"

cMentat::cMentat() {
	iMentatSentence = -1;

	TIMER_Speaking = -1;
	TIMER_Mouth = 0;
	TIMER_Eyes = 0;
	TIMER_Other = 0;

	iMentatMouth = 3;
	iMentatEyes = 3;
	iMentatOther = 0;

	memset(sentence, 0, sizeof(sentence));
}

cMentat::~cMentat() {
	iMentatSentence = -1;

	TIMER_Speaking = -1;
	TIMER_Mouth = 0;
	TIMER_Eyes = 0;
	TIMER_Other = 0;

	iMentatMouth = 3;
	iMentatEyes = 3;
	iMentatOther = 0;

	memset(sentence, 0, sizeof(sentence));
}

void cMentat::prepare(bool bTellHouse, int state, int house, int region) {
	if (bTellHouse)
	{
		if (house == ATREIDES) {
			INI_LOAD_BRIEFING(ATREIDES, 0, INI_DESCRIPTION);			
		} else if (house == HARKONNEN) {
			INI_LOAD_BRIEFING(HARKONNEN, 0, INI_DESCRIPTION);			
		} else if (house == ORDOS) {
			INI_LOAD_BRIEFING(ORDOS, 0, INI_DESCRIPTION);
		}
	} else {   
		if (state == GAME_BRIEFING) {
			INI_Load_scenario(house, region);
			INI_LOAD_BRIEFING(house, region, INI_BRIEFING);
		} else if (state == GAME_WINBRIEF) {
			if (rnd(100) < 50) {
				LOAD_SCENE("win01"); // ltank
			} else {
				LOAD_SCENE("win02"); // ltank
			}
			
			INI_LOAD_BRIEFING(house, region, INI_WIN);
		} else if (state == GAME_LOSEBRIEF)	{
			if (rnd(100) < 50) {
				LOAD_SCENE("lose01"); // ltank
			} else {
				LOAD_SCENE("lose02"); // ltank
			}
			INI_LOAD_BRIEFING(house, region, INI_LOSE);
		}
	}

	logbook("MENTAT: sentences prepared");
	iMentatSentence = -2;	// = sentence to draw and speak with (-1 = not ready, -2 means starting)
	TIMER_Speaking = 0;		// 0 means, set it up
}