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

class cSoundPlayer {

	public:
		cSoundPlayer(int maxVoices);
		~cSoundPlayer();

		void playSound(SAMPLE *sample, int pan, int vol);
		void playSound(int sampleId, int pan, int vol);

		// think about voices, clear voices, etc.
		void think();

	protected:
		void destroySound(int voice, bool force);

	private:
		int maximumVoices;
		int voices[256];

};

#endif /* CSOUNDPLAYER_H_ */
