#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class cMouseDrawer {
	public:
		cMouseDrawer(cPlayer * thePlayer);
		virtual ~cMouseDrawer();

		void draw();

		void onMouseAt(int x, int y);

		void drawToolTip();

	protected:
		void drawToolTipBackground();
		void drawToolTipGeneralInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);
		void drawToolTipWindTrapInformation(cWindTrap * theWindTrap, cTextWriter *textWriter);
		void drawToolTipSiloInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);
		void drawToolTipTurretInformation(cAbstractStructure * theStructure, cTextWriter *textWriter);

		int getDrawXToolTip(int width);
		int getDrawYToolTip(int height);
		int getWidthToolTip();
		int getHeightToolTip();

	private:
		cMouseToolTip * mouseToolTip;
		cPlayer * m_Player;
		int mouseX, mouseY;
};

#endif	/* CMOUSEDRAWER_H */

