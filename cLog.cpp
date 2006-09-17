/**

	Log class

	Responsible for anything that we want to log into log.txt

*/
#include "d2tmh.h"

void cLog::print(char *msg) {

	FILE *fp;
	fp = fopen("log.txt", "at");

	if (fp)
	{
		fprintf(fp, "%d| %s\n", game.iGameTimer, msg); 
		fclose(fp);
	}
}
