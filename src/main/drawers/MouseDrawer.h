#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class MouseDrawer {
	public:
		MouseDrawer(cPlayer *thePlayer, cMouse *theMouse);
		virtual ~MouseDrawer();

		void draw();

		void drawToolTip();

	protected:
		void drawToolTipBackground();
		void drawToolTipGeneralInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);
		void drawToolTipWindTrapInformation(cWindTrap * theWindTrap, cTextWriter *textWriter);
		void drawToolTipSiloInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);
		void drawToolTipTurretInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);

		void drawMouseTile();
		void drawMouseRectangle();

		int getDrawXToolTip(int width);
		int getDrawYToolTip(int height);
		int getWidthToolTip();
		int getHeightToolTip();

	private:
		cMouseToolTip * mouseToolTip;
		cMouse *mouse;
		cPlayer *player;
		cGameControlsContext * context;
};

#endif	/* CMOUSEDRAWER_H */

