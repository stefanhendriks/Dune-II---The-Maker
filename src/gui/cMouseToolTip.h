/*
 * cMouseToolTip.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#pragma once

class cPlayer;

class cMouseToolTip {
	public:
		cMouseToolTip(cPlayer *thePlayer);
		~cMouseToolTip();

	protected:

	private:
		cPlayer * player;

};

