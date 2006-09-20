/* 

	Dune II - The Maker

	Author : Stefan Hendriks
	Contact: stefan@dune2k.com / stefanhen83@planet.nl
	Website: http://d2tm.duneii.com

	2001 - 2006 (c) code by Stefan Hendriks

	Mentat class

	The mentat class encapsulates all mentat specific functions. 
	This is a global class. In the Mentat directory there will be:

	c<anyhouse>Mentat.cpp / c<anyhouse>Mentat.h which will be derived from this class.

	There is only one mentat in-game shown to the player. Therefor a good creation/deletion of it (when switching
	houses) is essential.
*/

class cMentat {

private:
	virtual void draw_mouth()=0;
	virtual void draw_eyes()=0;
	virtual void draw_other()=0;

	// Timed animation
	int TIMER_Mouth;
	int TIMER_Eyes;
	int TIMER_Other;
	int TIMER_Speaking;
	
	// draw 2 sentences at once, so 0 1, 2 3, 4 5, 6 7, 8 9
	char sentence[10][255];

	bool bReadyToSpeak;
	bool bWaitingForAnswer;

	int iMentatSentence;	
	int iMentatMouth;
	int iMentatEyes;
	int iMentatOther;

	void thinkSpeaking();
	void thinkMouth();
	void thinkEyes();
	void thinkOther();
public:
	cMentat();
	~cMentat();

	virtual void draw()=0;
	
	int getMentatMouth();
	int getMentatEyes();
	int getMentatOther();
	int getMentatSentence();
	int getTimerSpeaking();

	void init();
	void think();
	void setReadyToSpeak(bool value);
	void drawSentences();
	void prepare(bool bTellHouse, int state, int house, int region);
	void prepare(int house);
	bool shouldDrawSentences();
	bool isWaitingForAnAnswer();
	void setSentence(int index, char sent[255]);
};


