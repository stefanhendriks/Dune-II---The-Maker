/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

// TODO: Clean this class up big time.


class cGame {

public:

	cGame();

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
    int screen_x;
    int screen_y;
    int ini_screen_width;
    int ini_screen_height;
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
    int map_width;
    int map_height;
    int iRegion;
    int iMission;
    int iHouse;
    int selected_structure;
    int hover_unit;
    int fade_select;
    bool bFadeSelectDir;
    int paths_created;
    int iMusicVolume;
    int TIMER_movie;
    int iMovieFrame;
    int shake_x;
    int shake_y;
    int TIMER_shake;
    int iMusicType;
    int iWinQuota;
    void think_winlose();
    void winning();
    void losing();
    void think_movie();
    bool bPlaceIt;
    bool bPlacedIt;
    void setup_players();
    void think_message();
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
    bool isState(int thisState);
    void setState(int thisState);
    int getMaxVolume()
    {
        return iMaxVolume;
    }

    cSoundPlayer *getSoundPlayer()
    {
        return soundPlayer;
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
    void draw_movie(int iType);
    bool isResolutionInGameINIFoundAndSet();
    void setScreenResolutionFromGameIniSettings();
    int state;

	int iMaxVolume;

	cSoundPlayer *soundPlayer;
};

