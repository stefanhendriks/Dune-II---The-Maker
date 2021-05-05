#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class cMouseDrawer {
	public:
		cMouseDrawer(cPlayer * thePlayer);
		virtual ~cMouseDrawer();

		void draw();

		void drawToolTip();

        void onNotify(const s_MouseEvent &event);

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
        void onMouseAt(const s_MouseEvent &event);

		cMouseToolTip * mouseToolTip;
		cPlayer * player;
		int mouseX, mouseY;
};

#endif	/* CMOUSEDRAWER_H */

