#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

using namespace std;

// use nameless namespace so we can use the Exceptions defined here
// across multiple files. Avoids LNK2005 linking error.
// See also:
// http://stackoverflow.com/questions/10046485/error-lnk2005-already-defined
namespace {

	class NullArgumentException: public exception {
	
		virtual const char* what() const throw() {
			return "Argument may not be NULL.";
		}

	} NullArgumentException;


	class CannotLoadDataException: public exception {

		virtual const char * what() const throw() {
			return "Failed to load data.";
		}
	} CannotLoadDataException;

	class CannotLoadBitmapException: public exception {

		virtual const char * what() const throw() {
			return "Failed to load bitmap.";
		}
	} CannotLoadBitmapException;

}

#endif