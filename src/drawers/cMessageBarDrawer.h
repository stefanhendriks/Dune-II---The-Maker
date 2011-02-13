/*
 * cMessageBarDrawer.h
 *
 *  Created on: 9-nov-2010
 *      Author: Stefan
 */

#ifndef CMESSAGEBARDRAWER_H_
#define CMESSAGEBARDRAWER_H_

class cMessageBarDrawer {
	public:
		cMessageBarDrawer(cMessageBar * theMessageBar);
		cMessageBarDrawer();
		~cMessageBarDrawer();

		void drawMessageBar();
		cMessageBar * getMessageBar() { return messageBar; }

	protected:

	private:
		cMessageBar * messageBar;
		bool constructedMessageBar;
};

#endif /* CMESSAGEBARDRAWER_H_ */
