/* 
 * File:   cKeyboardManager.h
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

#ifndef CKEYBOARDMANAGER_H
#define	CKEYBOARDMANAGER_H

class cKeyboardManager {
	public:
		cKeyboardManager();
		cKeyboardManager(const cKeyboardManager& orig);
		virtual ~cKeyboardManager();
		void interact();
	private:
		void DEBUG_KEYS();
		void GAME_KEYS();
};

#endif	/* CKEYBOARDMANAGER_H */

