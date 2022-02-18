#pragma once

#include <string>

class BITMAP;

namespace messages {
    enum eMessageDrawerState {
        COMBAT,
        NEXT_CONQUEST
    };

    enum eMessageDrawerFadingState {
        FADE_IN,
        FADE_OUT
    };
}

class cMessageDrawer {
	public:
		cMessageDrawer();
		~cMessageDrawer();

		void draw();

		void thinkFast(); // timer based thinking to fade in/out message

		void init();
		void destroy();

        void setKeepMessage(bool value);

		void initRegionPosition(int offsetX, int offsetY);

		void initCombatPosition();

		void setMessage(const std::string &msg);  // set a new message
		const std::string& getMessage() { return cMessage; }
		bool hasMessage() { return (!cMessage.empty()); }

	protected:

	private:
		// MESSAGING / HELPER
		std::string cMessage;
		int iMessageAlpha;
		int TIMER_message;
		int x, y;	// draw positions
		BITMAP *bmpBar;
		BITMAP *temp;

        messages::eMessageDrawerState state;
        messages::eMessageDrawerFadingState fadeState;

        bool keepMessage;

        void createMessageBarBmp(int desiredWidth);
};
