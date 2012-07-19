#pragma once
#ifndef DATA_H
#define DATA_H

#include "../exceptions/exceptions.h"
#include "Bitmap.h"

#include "../include/allegroh.h"

class Data {

	public:
		Data(DATAFILE * dataFile) {
			if (dataFile == NULL) throw NullArgumentException;
			this->dataFile = dataFile;
		}

		~Data() {
			unload_datafile(this->dataFile);
			this->dataFile = NULL;
		}

		Bitmap * getBitmap(int index) {
			BITMAP * bmp = (BITMAP *) dataFile[index].dat;
			return new Bitmap(bmp, true);
		}

	private:
		DATAFILE * dataFile;

};

#endif