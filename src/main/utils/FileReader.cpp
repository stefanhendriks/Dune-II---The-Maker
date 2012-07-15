#include "FileReader.h"

FileReader::FileReader(const char * pathToFile) {
	stream.open(pathToFile);
}

FileReader::~FileReader() {
	if (stream.is_open()) {
		stream.close();
	}
}

bool FileReader::hasNext() {
	if (!stream.is_open()) {
		return false;
	}
	return stream.good();
}

std::string FileReader::getLine() {
	if (!stream.is_open()) {
		return "";
	}
	std::string line;
 	std::getline(stream, line);
 	return line;
}