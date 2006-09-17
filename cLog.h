/**

	Log class

	Responsible for anything that we want to log into log.txt

*/

class cLog {

private:
	void printTitle();
	bool titlePrinted;
public:
	cLog();
	
	void printVersion();
	void print(char *msg);
};