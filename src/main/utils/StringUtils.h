#pragma once
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <stdio.h>
#include <stdlib.h>

class StringUtils {

public:

	static int getNumberFromString(const std::string & str) {
		return strtol (str.c_str(), NULL, 10);
	}

	static long getLongFromString(const std::string & str) {
		return strtol (str.c_str(), NULL, 10);
	}

};

#endif