#ifndef VERSION_H
#define VERSION_H

#include <string>
#include "stdio.h"

class Version {
	public:
		Version(int major, int minor, int patch) {
			this->major = major;
			this->minor = minor;
			this->patch = patch;
		}

	std::string asString() {
		char version[255];
		sprintf(version, "Version %d.%d.%d", major, minor, patch);
		return std::string(version);
	}

	private:
		int major;
		int minor;
		int patch;
};
#endif