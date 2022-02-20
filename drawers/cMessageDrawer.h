#pragma once

#include <string>
#include "utils/cPoint.h"

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

		void setMessage(const std::string & msg, bool keepMessage = false);
		std::string getMessage() { return m_message; }
		bool hasMessage() { return !m_message.empty(); }

	protected:

	private:
		std::string m_message;
        bool m_keepMessage;
		int m_TIMER_message;
        int m_alpha;

        cPoint m_position; // draw positions
		BITMAP *m_bmpBar;
		BITMAP *m_temp;

        int m_timeMessageIsVisible;

        messages::eMessageDrawerState m_state;
        messages::eMessageDrawerFadingState m_fadeState;

        void createMessageBarBmp(int desiredWidth);
};
