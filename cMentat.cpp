/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"

cMentat::cMentat() {
	init();
}

cMentat::~cMentat() {
	init();
}

void cMentat::init() {	
	iMentatSentence = -1;

	TIMER_Speaking = -1;
	TIMER_Mouth = 0;
	TIMER_Eyes = 0;
	TIMER_Other = 0;

	iMentatMouth = 3;
	iMentatEyes = 3; 
	iMentatOther = 0;

	bReadyToSpeak = false;
	bWaitingForAnswer = false;

	memset(sentence, 0, sizeof(sentence));
}

void cMentat::prepare(int house) {
	if (house > -1) {
		prepare(true, 0, house, 0);
	}
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
	Logger.print("MENTAT: sentences prepared - ready to speak");
	
	// On purpose the iMentatSentence is set to -2; when timer_speaking is at 0
	// the first 2 sentences will be read from they array and the wait time calculated.
	iMentatSentence = -2;	
	TIMER_Speaking = 0;		
}

// Remember: called by TimeManager (Global Timer)
void cMentat::think() {
	thinkSpeaking();
	thinkMouth();
	thinkEyes();
	thinkOther();
}

void cMentat::thinkSpeaking() {
	if (!bReadyToSpeak) {
		return;
	}
	
	if (TIMER_Speaking > 0) {
		TIMER_Speaking--;
		/* whoever puts a return here because he thinks he is smart, will be thrown in a sand pit **/
	}
	
	if (TIMER_Speaking == 0 || Mouse.btnSingleClickLeft()) {
		/** Next sentences to read for the player **/
		if (iMentatSentence < 8) {
			iMentatSentence += 2; 
		} else {
			bWaitingForAnswer = true;			
		}
		// Calculate next time before we show next sentences. 
		// Based on length of text.
		int iLength = strlen(sentence[iMentatSentence]);
		iLength += strlen(sentence[iMentatSentence + 1]);

		TIMER_Speaking = iLength * 12;		

		iMentatMouth = 0; // force mentat to close mouth

		if (iLength < 2) {
			bWaitingForAnswer = true;
			return;
		}
	}
}

void cMentat::thinkEyes() {
	if (TIMER_Eyes > 0)	{
		TIMER_Eyes--;
	} else {
		int iWas = iMentatEyes;

		int iChance = rnd(100);
		if (iChance < 30) {
			iMentatEyes = 3;		
		} else if (iChance >= 30 && iChance < 60) {
			iMentatEyes = 0;
		} else if (iChance > 60) {
			iMentatEyes=4;				
		}

		/** When it is the same, bail out and retry next frame **/
		if (iMentatEyes == iWas) {
			return;
		}

		if (iMentatEyes != 4) {
			TIMER_Eyes = 90 + rnd(160); 
		} else {
			TIMER_Eyes = 30;
		}
	}
}

void cMentat::thinkOther() {
	if (TIMER_Other > 0)	{
		TIMER_Other--;
	} else {
		iMentatOther = rnd(5); 		
	}
}

void cMentat::thinkMouth() {
	if (TIMER_Mouth > 0) {
		TIMER_Mouth--;
	} else if (TIMER_Mouth == 0) {		
		if (TIMER_Speaking > 0) {
			int iOld = iMentatMouth;

			// when mouth is shut, perhaps wait a bit.
			if (iMentatMouth == 0) {				
				if (rnd(100) > 55) {
					TIMER_Mouth = 15 + rnd(20);
				}
			}

			if (TIMER_Mouth == 0) {
				iMentatMouth += (1 + rnd(4));
			}

			// correct any frame
			if (iMentatMouth > 4) {
				iMentatMouth-=5;
			}

			/** in case we do not wait **/
			if (TIMER_Mouth == 0)
			{
				/** it is still the same - change it **/
				if (iMentatMouth == iOld) {
					iMentatMouth++;
				}

				// correct if nescesary:
				if (iMentatMouth > 4) {
					iMentatMouth -= 5;
				}

				TIMER_Mouth += 20 + rnd(20);
			}
		} else {
			iMentatMouth = 0; /** stop animation when no sentence is available **/

		}
	}
}

int cMentat::getMentatMouth() {
	return iMentatMouth;
}

int cMentat::getMentatEyes() {
	return iMentatEyes;
}

int cMentat::getMentatOther() {
	return iMentatOther;
}

int cMentat::getMentatSentence() {
	return iMentatSentence;
}

int cMentat::getTimerSpeaking() {
	return TIMER_Speaking;
}

bool cMentat::isWaitingForAnAnswer() {
	return bWaitingForAnswer;
}
bool cMentat::shouldDrawSentences() {
	if (iMentatSentence > -1 && iMentatSentence < 10 && bReadyToSpeak) {
		return true;
	}

	return false;
}

void cMentat::drawSentences() {		
	if (shouldDrawSentences()) {
		assert(iMentatSentence < 10);	// these should 
		assert(iMentatSentence > -1);	// never fail

		alfont_textprintf(bmp_screen, bene_font, 17,17, makecol(0,0,0), "%s", sentence[iMentatSentence]);
		alfont_textprintf(bmp_screen, bene_font, 16,16, makecol(255,255,255), "%s", sentence[iMentatSentence]);
		alfont_textprintf(bmp_screen, bene_font, 17,33, makecol(0,0,0), "%s", sentence[iMentatSentence+1]);
		alfont_textprintf(bmp_screen, bene_font, 16,32, makecol(255,255,255), "%s", sentence[iMentatSentence+1]);
	}
}

void cMentat::setSentence(int index, char sent[255]) {
	sprintf(sentence[index], "%s", sent);	
}

void cMentat::setReadyToSpeak(bool value) {
	bReadyToSpeak = value;
}

/*

// mentat mouth
	if (TIMER_mentat_Mouth <= 0) {
		TIMER_mentat_Mouth = 13+rnd(5);
	}

    if (TIMER_mentat_Speaking < 0 && iType > -1)
    {        
        // NO
		
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_REPEAT].dat, 293, 423);
		if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468)) {
            if (Mouse.btnSingleClickLeft())
            {
                // head back to choose house
                iMentatSpeak=-1; // prepare speaking
                //state = GAME_HOUSE;
            }
		}

        // YES/PROCEED
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_PROCEED].dat, 466, 423);
		
		if ((mouse_x > 446 && mouse_x < 619) && (mouse_y >423 && mouse_y < 468)) {
            if (Mouse.btnSingleClickLeft())
            {
                if (isState(GAME_BRIEFING))
                {
                // proceed, play mission
                state = GAME_PLAYING;
                
                // CENTER MOUSE
                position_mouse(320, 240);
                                
                bFadeOut=true;
                
                play_music(MUSIC_PEACE);
                }
                else if (state == GAME_WINBRIEF)
                {
                //
					if (bSkirmish)
					{
						state = GAME_SETUPSKIRMISH;
						play_music(MUSIC_MENU);
					}
					else
					{
					
                    state = GAME_REGION;
                    REGION_SETUP(game.iMission, game.iHouse);

                    
                    // PLAY THE MUSIC
                    play_music(MUSIC_CONQUEST);
					}

					// PREPARE NEW MENTAT BABBLE
                    iMentatSpeak=-1;
                    
                    bFadeOut=true;
                }
                else if (state == GAME_LOSEBRIEF)
                {
                //
					if (bSkirmish)
					{
						state = GAME_SETUPSKIRMISH;
						play_music(MUSIC_MENU);
					}
					else
					{
						if (game.iMission > 1)
						{
							state = GAME_REGION;

							game.iMission--; // we did not win
							REGION_SETUP(game.iMission, game.iHouse);

							// PLAY THE MUSIC
							play_music(MUSIC_CONQUEST);
						}
						else
						{
							state = GAME_BRIEFING;
							play_music(MUSIC_BRIEFING);
						}
					
					}
                    // PREPARE NEW MENTAT BABBLE
                    iMentatSpeak=-1;
                                        
                    bFadeOut=true;
                }

			} 			
		}

    }    


	*/


/* 

// draw buttons
    if (TIMER_mentat_Speaking < 0)
    {
		rectfill(bmp_screen, 20, 20, 520, 320, makecol(25,25,255));
        // NO
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_NO].dat, 293, 423);

		if ((mouse_x > 293 && mouse_x < 446) && (mouse_y > 423 && mouse_y < 468)) {
            if (Mouse.btnSingleClickLeft())
            {
                // head back to choose house
                iHouse=-1;
                state = GAME_HOUSE;
                bFadeOut=true;
         	}
		}
        // YES
        draw_sprite(bmp_screen, (BITMAP *)gfxmentat[BTN_YES].dat, 466, 423);
        if ((mouse_x > 446 && mouse_x < 619) && (mouse_y >423 && mouse_y < 468))
            if (Mouse.btnSingleClickLeft())
            {
                // yes!
                state = GAME_BRIEFING; // briefing
                iMission = 1;
                iRegion  = 1;
                iMentatSpeak=-1; // prepare speaking

                player[0].set_house(iHouse);
                
                // play correct mentat music
                play_music(MUSIC_BRIEFING);
                bFadeOut=true;               
                
            }
        

    }    
*/