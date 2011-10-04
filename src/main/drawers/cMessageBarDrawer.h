#ifndef CMESSAGEBARDRAWER_H_
#define CMESSAGEBARDRAWER_H_

class cMessageBarDrawer {
	public:
		cMessageBarDrawer(MessageBar * theMessageBar);
		cMessageBarDrawer();
		~cMessageBarDrawer();

		void drawMessageBar();

		MessageBar * getMessageBar() {
			return messageBar;
		}

		void think();

		void setMessage(const char msg[255]);

	protected:

	private:
		MessageBar * messageBar;
};

#endif /* CMESSAGEBARDRAWER_H_ */
