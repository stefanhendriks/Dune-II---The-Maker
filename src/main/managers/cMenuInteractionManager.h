#ifndef CMENUINTERACTIONMANAGER_H_
#define CMENUINTERACTIONMANAGER_H_

#include "../player/cPlayer.h"
#include "cInteractionManager.h"

class cMenuInteractionManager : public cInteractionManager {
	public:
		cMenuInteractionManager(cPlayer * thePlayer);
		virtual ~cMenuInteractionManager();

		void interactWithMouse();

	protected:

	private:
};

#endif /* CMENUINTERACTIONMANAGER_H_ */
