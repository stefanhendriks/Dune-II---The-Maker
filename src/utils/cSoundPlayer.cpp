#include "cSoundPlayer.h"

#include "definitions.h"
#include "utils/cLog.h"

#include <allegro/datafile.h>
#include <allegro/digi.h>
#include <allegro/midi.h>
#include <allegro/sound.h>
#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>

namespace {

constexpr int kAllegroMaxNrVoices = 256; // From the Allegro 4 documentation
constexpr int kAllegroMaxVolume = 255;
constexpr int kAllegroPanCenter = 128;

constexpr int kFinishedPLaying = -1;
constexpr int kNoVoiceAvailable = -1;
constexpr int kNoVoice = -1;
constexpr int kNoLoop = 0;

constexpr int kMinNrVoices = 4;
constexpr int kMaxVolume = 220;

}

class cSoundData {
public:
    cSoundData() {
        auto logger = cLogger::getInstance();

        gfxaudio = load_datafile("data/gfxaudio.dat");
        if (gfxaudio == nullptr) {
            static auto msg = "Could not hook/load datafile: gfxaudio.dat.";
            logger->log(LOG_ERROR, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
            throw std::runtime_error(msg);
        } else {
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Hooked datafile: gfxaudio.dat.", OUTC_SUCCESS);
        }
    }

    const SAMPLE* getSample(int sampleId) const {
        return static_cast<const SAMPLE*>(gfxaudio[sampleId].dat);
    }

    MIDI* getMusic(int musicId) const {
        return static_cast<MIDI*>(gfxaudio[musicId].dat);
    }
private:
    const DATAFILE* gfxaudio;
};

cSoundPlayer::cSoundPlayer(const cPlatformLayerInit& init) : cSoundPlayer(init, kAllegroMaxNrVoices) {
}

cSoundPlayer::cSoundPlayer(const cPlatformLayerInit&, int maxNrVoices)
        : soundData(std::make_unique<cSoundData>()) {
    // The platform layer init object is not used here, but since it needs to be passed, it tells
    // the caller that the initialization needs to be performed first.

    auto logger = cLogger::getInstance();

    if (maxNrVoices < kMinNrVoices) {
        logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Muting all sound.", OUTC_SUCCESS);
        return;
    }

    int nr_voices = maxNrVoices;
    while (true) {
        if (nr_voices < kMinNrVoices) {
            logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
            return;
        }

        reserve_voices(nr_voices, 0); // Reserve nothing for MIDI, assume it will "steal" from the digi voices

        if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, nullptr) == 0)
        {
            auto msg = fmt::format("Successfully installed sound. {} voices reserved", nr_voices);
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);

            // One fewer voice for the samples, as MIDI playing will use a voice.
            voices.resize(nr_voices - 1, kNoVoice);
            break;
        }

        auto msg = fmt::format("Failed reserving {} voices. Will try {}.", nr_voices, (nr_voices / 2));
        logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_FAILED);

        nr_voices /= 2;
    }

    // Sound effects are loud, the music is queiter (its background music, so it should not be disturbing).
    // FIXME: shouldn't this be the Allegro max volume and half of that instead of 220 and 110?
    set_volume(kMaxVolume, kMaxVolume / 2);
}

cSoundPlayer::~cSoundPlayer() {
    for (int voice : voices) {
        if (voice != kNoVoice) {
            deallocate_voice(voice);
        }
    }
}

int cSoundPlayer::getMaxVolume() {
    // TODO: This will become configurable (so you can set your own max volume for sounds, etc)
    return kAllegroMaxVolume;
}

void cSoundPlayer::think() {
    for (int& voice : voices) {
        if (voice == kNoVoice) {
            continue;
        }

        auto pos = voice_get_position(voice);
        if (pos == kFinishedPLaying) {
            deallocate_voice(voice); // Same as release_voice, as it stopped playing
            voice = kNoVoice;
        }
    }
}

void cSoundPlayer::playSound(int sampleId) {
    playSound(sampleId, kAllegroMaxVolume);
}

void cSoundPlayer::playSound(int sampleId, int vol) {
    if (vol <= 0) {
        return;
    }

    if (voices.size() < kMinNrVoices) {
        return;
    }


    auto voice = std::find(voices.begin(), voices.end(), kNoVoice);
    if (voice == voices.end()) {
        cLogger::getInstance()->log(eLogLevel::LOG_WARN, eLogComponent::COMP_SOUND, "Playing sound",
                                    "Cannot play sample, no more voices available.");
        return;
    }

    vol = std::clamp(vol, 0, kAllegroMaxVolume);

    const SAMPLE* sample = soundData->getSample(sampleId);
	assert(sample);

	*voice = allocate_voice(sample);
    assert(*voice != kNoVoiceAvailable);

    voice_set_playmode(*voice, PLAYMODE_PLAY | PLAYMODE_FORWARD); // The default
    voice_set_volume(*voice, vol);
    voice_set_pan(*voice, kAllegroPanCenter);
    voice_start(*voice);
}

void cSoundPlayer::playVoice(int sampleId, int house) {
	if (house == HARKONNEN) {
		sampleId += 1;
	} else if (house == ORDOS) {
		sampleId += 2;
	}

    playSound(sampleId);
}

void cSoundPlayer::playMusic(int sampleId) {
    play_midi(soundData->getMusic(sampleId), kNoLoop);
}

bool cSoundPlayer::isMusicPlaying() const {
    return midi_pos > -1;
}

void cSoundPlayer::stopMusic() {
    stop_midi();
}

void cSoundPlayer::setMusicVolume(int vol)
{
    int digivol;
    int midivol;
    get_volume(&digivol, &midivol);
    set_volume(digivol, vol);
};

void cSoundPlayer::changeMusicVolume(int delta)
{
    int digivol;
    int midivol;
    get_volume(&digivol, &midivol);
    int vol = std::clamp(digivol + delta, 0, kAllegroMaxVolume);
    set_volume(digivol, vol);
};
