#ifndef CMESSAGEBARDRAWER_H_
#define CMESSAGEBARDRAWER_H_

class cMessageBarDrawer {
	public:
		cMessageBarDrawer(cMessageBar * theMessageBar);
		cMessageBarDrawer();
		~cMessageBarDrawer();

		void drawMessageBar();

		cMessageBar * getMessageBar() {
			return messageBar;
		}

		void think();

		void setMessage(const char msg[255]);

	protected:

	private:
		cMessageBar * messageBar;
};

#endif /* CMESSAGEBARDRAWER_H_ */
