/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

class cGame {

	public:

		cGame();
		~cGame();

		std::string game_filename;

		bool windowed;
		char version[15];
		int iAlphaScreen;
		int iFadeAction;
		int iRegionState;
		int iRegionScene;
		int iRegionSceneAlpha;
		int iRegionConquer[MAX_REGIONS];
		int iRegionHouse[MAX_REGIONS];
		char cRegionText[MAX_REGIONS][255];
		bool bPlaySound;
		bool bMp3;
		bool bPlaying;
		bool bSkirmish;
		int iSkirmishMap;
		int screenshot;
		int iSkirmishStartPoints;
		void init();
		void mission_init();
		void run();
		int iRegion;
		int iMission;
		int iHouse;
		int selected_structure;
		int hover_unit;
		int fade_select;
		bool bFadeSelectDir;
		int paths_created;
		int iMusicVolume;
		int shake_x;
		int shake_y;
		int TIMER_shake;
		int iMusicType;
		int iWinQuota;
		void think_winlose();
		void winning();
		void losing();
		bool bPlaceIt;
		bool bPlacedIt;

		void think_music();
		char mentat_sentence[10][255];
		int TIMER_mentat_Speaking;
		int iMentatSpeak;
		int iMentatMouth;
		int iMentatEyes;
		int iMentatOther;
		int TIMER_mentat_Mouth;
		int TIMER_mentat_Eyes;
		int TIMER_mentat_Other;
		void MENTAT_draw_mouth(int iMentat);
		void MENTAT_draw_eyes(int iMentat);
		void MENTAT_draw_other(int iMentat);
		void think_mentat();
		void FADE_OUT();
		void preparementat(bool bTellHouse);
		bool setupGame();
		void shutdown();
		bool isState(GameState theState);
		void setState(GameState theState);

		int getMaxVolume() {
			return iMaxVolume;
		}

		cSoundPlayer *getSoundPlayer() {
			return soundPlayer;
		}

		cMoviePlayer *getMoviePlayer() {
			return moviePlayer;
		}

		void setMoviePlayer(cMoviePlayer * value) {
			moviePlayer = value;
		}

		cScreenResolution *getScreenResolution() {
			return screenResolution;
		}

		void setScreenResolution(cScreenResolution * newScreenResolution) {
			screenResolution = newScreenResolution;
		}

		cScreenResolution *getScreenResolutionFromIni() {
			return screenResolutionFromIni;
		}

		void setScreenResolutionFromIni(cScreenResolution * newScreenResolution) {
			screenResolutionFromIni = newScreenResolution;
		}

		void draw_placeit();
		void combat_mouse();
		int getGroupNumberFromKeyboard();
		void destroyAllUnits(bool);
		void destroyAllStructures(bool);

	private:
		void poll();
		void combat();
		void draw_mentat(int iType);
		void setup_skirmish();
		void house();
		void tellhouse();
		void mentat(int iType);
		void menu();
		void region();
		void runGameState();
		void shakeScreenAndBlitBuffer();
		void handleTimeSlicing();
		bool isResolutionInGameINIFoundAndSet();
		void setScreenResolutionFromGameIniSettings();

		bool playerHasAnyStructures(int iPlayerId);
		bool playerHasAnyGroundUnits(int iPlayerId);
		bool isWinQuotaSet();
		bool playerHasMetQuota(int iPlayerId);

		GameState state;

		cScreenResolution * screenResolution;
		cScreenResolution * screenResolutionFromIni;

		int iMaxVolume;

		cSoundPlayer *soundPlayer;
		cMoviePlayer *moviePlayer;

};

