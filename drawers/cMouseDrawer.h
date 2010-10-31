/*
 * File:   cMouseDrawer.h
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class cMouseDrawer {
	public:
		cMouseDrawer(cPlayer *thePlayer, cMouse *theMouse);
		cMouseDrawer(const cMouseDrawer& orig);
		virtual ~cMouseDrawer();

		void draw();

		void drawToolTip();

	private:
		cMouseToolTip * mouseToolTip;
		cMouse *mouse;
		cPlayer *player;
};

#endif	/* CMOUSEDRAWER_H */

