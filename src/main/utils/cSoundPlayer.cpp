#include "cSoundPlayer.h"

cSoundPlayer::cSoundPlayer(int maximumVoices, int digiVolume, int midiVolume) {
	assert(digiVolume <= SOUNDPLAYER_MAX_VOLUME);
	assert(midiVolume <= SOUNDPLAYER_MAX_VOLUME);
	assert(maximumVoices < SOUNDPLAYER_MAX_VOICES);
	assert(maximumVoices >= 0);
	this->maximumVoices = maximumVoices;
	this->digiVolume = digiVolume;
	this->midiVolume = midiVolume;
	this->datafile = NULL;
	this->voices.resize(maximumVoices);
	for (int i = 0; i < voices.size(); i++) {
		voices[i] = -1;
	}
}

cSoundPlayer::~cSoundPlayer() {
}

void cSoundPlayer::think() {
	for (int i = 0; i < voices.size(); i++) {

		int pos;
		int voice = voices[i];

		//if it contains a voice get its position
		if (voice != -1) {
			pos = voice_get_position(voice);
		} else {
			pos = -2;
		}

		//if it is at the end then release it
		if (pos == -1) {
			destroySound(voice, true);
		}
	}
}

int cSoundPlayer::getAvailableVoices() {
	int availableVoices = 0;
	for (int i = 0; i < voices.size(); i++) {
		if (voices[i] == -1) {
			availableVoices++;
		}
	}
	return availableVoices;
}

void cSoundPlayer::destroySound(int voiceId, bool force) {
	if (voiceId < 0) {
		return;
	}
	int voice = voices.at(voiceId);
	if (voice < 0) {
		return;
	}

	if (force) {
		deallocate_voice(voice);
	} else {
		release_voice(voice);
	}

	for (int i = 0; i < voices.size(); i++) {
		if (voices.at(i) == voiceId) {
			voices[i] = -1;
			break;
		}
	}

}

void cSoundPlayer::playSound(int sampleId, int pan, int vol) {
	if (datafile[sampleId].dat == NULL) {
		return;
	}
	playSound((SAMPLE *) datafile[sampleId].dat, pan, vol);
}

void cSoundPlayer::playMidi(int midiId) {
	if (datafile[midiId].dat == NULL) {
		return;
	}
	play_midi((MIDI *) datafile[midiId].dat, 0);
}

void cSoundPlayer::playSound(SAMPLE *sample, int pan, int vol) {
	if (vol < 0) {
		return;
	}

	assert(sample);
	assert(pan >= SOUNDPLAYER_PAN_MIN && pan < SOUNDPLAYER_PAN_MAX);
	assert(vol <= SOUNDPLAYER_MAX_VOLUME);

	// allocate voice
	int voice = allocate_voice(sample);

	if (voice != -1) {
		voice_set_playmode(voice, 0);
		voice_set_volume(voice, vol);
		voice_set_pan(voice, pan);
		voice_start(voice);
		for (int i = 0; i < voices.size(); i++) {
			if (voices[i] == -1) {
				voices[i] = voice;
				break;
			}
		}
	}
}
