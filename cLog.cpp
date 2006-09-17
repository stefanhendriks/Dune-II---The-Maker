/**

	Log class

	Responsible for anything that we want to log into log.txt

*/
#include "d2tmh.h"

cLog::cLog() {
	titlePrinted = false;
}

void cLog::print(char *msg) {
	if (!titlePrinted) {
		printTitle();
	}

	FILE *fp;
	fp = fopen("log.txt", "at");

	if (fp)
	{
		fprintf(fp, "%d| %s\n", game.iGameTimer, msg); 
		fclose(fp);
	}
}

/** 
Print header/title of log file, should be done at start 
*/
void cLog::printTitle() {
	FILE *fp;
	fp = fopen("log.txt", "wt");

	if (fp) 
	{
		fprintf(fp, "DUNE II - The Maker\n");
		fprintf(fp, "-------------------\n\n"); // print head of logbook
		fclose(fp);
	}

	titlePrinted = true;
}

void cLog::printVersion() {
	Logger.print("-------------------");
	Logger.print("Version information");
	Logger.print("-------------------");	
	Logger.print(game.version);
	char msg[255];
	sprintf(msg, "Compiled at %s , %s", __DATE__, __TIME__);
	Logger.print(msg); 
}