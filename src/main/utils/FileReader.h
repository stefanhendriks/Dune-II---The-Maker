#ifndef FILE_READER_H
#define FILE_READER_H

#include <iostream>
#include <fstream>
#include <string>

class FileReader {

	public:
		FileReader(const char * pathToFile);
		~FileReader();

		bool hasNext();

		std::string getLine();

	private:
		std::ifstream stream;

};

#endif