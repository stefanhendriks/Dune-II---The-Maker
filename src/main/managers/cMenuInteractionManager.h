/*
 * cMenuInteractionManager.h
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#ifndef CMENUINTERACTIONMANAGER_H_
#define CMENUINTERACTIONMANAGER_H_

class cMenuInteractionManager : public cInteractionManager {
	public:
		cMenuInteractionManager(cPlayer * thePlayer);
		virtual ~cMenuInteractionManager();

		void interactWithMouse();

	protected:

	private:
};

#endif /* CMENUINTERACTIONMANAGER_H_ */
