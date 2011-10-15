#ifndef CMOUSETOOLTIP_H_
#define CMOUSETOOLTIP_H_

class MouseToolTip {
	public:
		MouseToolTip(cPlayer *thePlayer, Mouse *theMouse);
		~MouseToolTip();

	protected:

	private:
		cPlayer * player;
		Mouse * mouse;

};

#endif /* CMOUSETOOLTIP_H_ */
