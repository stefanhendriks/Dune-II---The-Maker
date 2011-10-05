/*
 * cMouseToolTip.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSETOOLTIP_H_
#define CMOUSETOOLTIP_H_

class MouseToolTip {
	public:
		MouseToolTip(cPlayer *thePlayer, cMouse *theMouse);
		~MouseToolTip();

	protected:

	private:
		cPlayer * player;
		cMouse * mouse;

};

#endif /* CMOUSETOOLTIP_H_ */
