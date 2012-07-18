#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

using namespace std;

class NullArgumentException: public exception {
	
	virtual const char* what() const throw() {
		return "Argument may not be NULL.";
	}

} NullArgumentException;

#endif