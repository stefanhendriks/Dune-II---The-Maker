/*
 * cSoundPlayer.h
 *
 *  Created on: 6-aug-2010
 *      Author: Stefan
 *
 *  A class responsible for playing sounds. But also managing the sounds being played (ie, not
 *  playing too many sounds in the same frame, etc).
 *
 */

#ifndef CSOUNDPLAYER_H_
#define CSOUNDPLAYER_H_

#include <iostream>
#include <vector>

#include "allegro.h"

#define SOUNDPLAYER_MAX_VOLUME		256
#define SOUNDPLAYER_MAX_VOICES		256
#define	SOUNDPLAYER_PAN_MAX 		256
#define SOUNDPLAYER_PAN_MIN			0

class cSoundPlayer {

	public:
		cSoundPlayer(int maximumVoices, int theDigiVolume, int music);
		~cSoundPlayer();

		void playSound(SAMPLE *sample, int pan, int vol);
		void playSound(int sampleId, int pan, int vol);
		void playMidi(int midiId);

		void setDatafile(DATAFILE * datafile) { this->datafile = datafile; };

		// think about voices, clear voices, etc.
		void think();

		int getAvailableVoices();

		int getMaxVoices() { return maximumVoices; }
	protected:
		void destroySound(int voice, bool force);

	private:
		int maximumVoices;
		std::vector<int> voices;

		int midiVolume;
		int digiVolume;

		DATAFILE *datafile;
};

#endif /* CSOUNDPLAYER_H_ */
