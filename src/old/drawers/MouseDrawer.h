#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class MouseDrawer {
	public:
		MouseDrawer(cPlayer *thePlayer, Mouse *theMouse);
		virtual ~MouseDrawer();

		void draw();

		void drawToolTip();

	protected:
		void drawToolTipBackground();
		void drawToolTipGeneralInformation(cAbstractStructure * theStructure, TextWriter *textWriter);
		void drawToolTipWindTrapInformation(cWindTrap * theWindTrap, TextWriter *textWriter);
		void drawToolTipSiloInformation(cAbstractStructure * theStructure, TextWriter *textWriter);
		void drawToolTipTurretInformation(cAbstractStructure * theStructure, TextWriter *textWriter);

		void drawMouseTile();
		void drawMouseRectangle();

		int getDrawXToolTip(int width);
		int getDrawYToolTip(int height);
		int getWidthToolTip();
		int getHeightToolTip();

	private:
		MouseToolTip * mouseToolTip;
		Mouse *mouse;
		cPlayer *player;
		cGameControlsContext * context;
};

#endif	/* CMOUSEDRAWER_H */

