/*
 * cMouseToolTip.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSETOOLTIP_H_
#define CMOUSETOOLTIP_H_

class cMouseToolTip {
	public:
		cMouseToolTip(cPlayer *thePlayer, cMouse *theMouse);
		~cMouseToolTip();

	protected:

	private:
		cPlayer * player;
		cMouse * mouse;

};

#endif /* CMOUSETOOLTIP_H_ */
