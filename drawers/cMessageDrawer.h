#ifndef CMESSAGEDRAWER_H_
#define CMESSAGEDRAWER_H_

enum eMessageDraweState {
    COMBAT,
    NEXT_CONQUEST
};

class cMessageDrawer {
	public:
		cMessageDrawer();
		~cMessageDrawer();

		void draw();

		void think(); // timer based thinking to fade in/out message

		void init();
		void destroy();

		void initRegionPosition(int offsetX, int offsetY);

		void initCombatPosition();

		void setMessage(const char msg[255]); // set a new message
		char * getMessage() { return &cMessage[0]; }
		bool hasMessage() { return cMessage[0] != '\0'; }

	protected:

	private:
		// MESSAGING / HELPER
		char cMessage[255];
		int iMessageAlpha;
		int TIMER_message;
		int x, y;	// draw positions
		BITMAP *bmpBar;
		BITMAP *temp;

        eMessageDraweState state;

    void createMessageBarBmp(int desiredWidth);
};

#endif /* CMESSAGEDRAWER_H_ */
