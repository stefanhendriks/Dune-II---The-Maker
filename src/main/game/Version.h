#pragma once
#ifndef VERSION_H
#define VERSION_H

#include <string>
#include "stdio.h"

class Version {
	public:
		Version(int major, int minor, int patch, int revision) {
			this->major = major;
			this->minor = minor;
			this->patch = patch;
			this->revision = revision;
		}

	std::string asString() {
		char version[255];
		sprintf(version, "Version %d.%d.%d - revision %d", major, minor, patch, revision);
		return std::string(version);
	}

	private:
		int major;
		int minor;
		int patch;
		int revision;
};
#endif