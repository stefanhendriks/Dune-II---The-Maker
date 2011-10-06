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
