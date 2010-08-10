/*
 * cSoundPlayer.cpp
 *
 *  Created on: 6-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSoundPlayer::cSoundPlayer(int maxVoices) {
	maximumVoices = maxVoices;
	memset(voices, -1, sizeof(voices));
}

cSoundPlayer::~cSoundPlayer() {
	if (maximumVoices > -1) {
		for (int i = 0; i < maximumVoices; i++) {
			destroySound(i, true);
		}
	}
}

void cSoundPlayer::think() {
	if (maximumVoices < 0) return;

	for(int i = 0;i < maximumVoices; i++) {

		int pos;
		int voice = voices[i];

		//if it contains a voice get its position
		if(voice != -1) {
			pos = voice_get_position(voice);
		} else {
			pos = -2;
		}

		//if it is at the end then release it
		if(pos == -1)
		{
			destroySound(voice, true);
		}
	}
}

void cSoundPlayer::destroySound(int voice, bool force) {
	if (maximumVoices < 0) return;
	if (voice < 0) return;
	if (voices[voice] < 0) return;

	if (force) {
		deallocate_voice(voices[voice]);
	} else {
		release_voice(voices[voice]);
	}

	for (int i = 0; i < maximumVoices; i++) {
		if (voices[i] == voice) {
			voices[i] = -1;
			break;
		}
	}
}


void cSoundPlayer::playSound(SAMPLE *sample, int pan, int vol) {
	if (maximumVoices < 0) return;

	assert(sample);
	if (vol < 0) return;
	assert(pan >= 0 && pan < 256);
	assert(vol > 0 && vol <= 256);

	// allocate voice
	int voice = allocate_voice(sample);

	if (voice != -1) {
		voice_set_playmode(voice,0);
		voice_set_volume(voice, vol);
		voice_set_pan(voice, pan);
		voice_start(voice);
		for (int i = 0; i < maximumVoices; i++) {
			if (voices[i] == -1) {
				voices[i] = voice;
				break;
			}
		}
	}
}
