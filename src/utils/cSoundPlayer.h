#pragma once

#include "cPlatformLayerInit.h"
#include "data/gfxaudio.h" // Use IDs from this file to play samples

#include <memory>
#include <vector>

class cSoundData;

class cSoundPlayer {
	public:
        // Initialize the platform layer before creating this object.
		explicit cSoundPlayer(const cPlatformLayerInit& init);
		cSoundPlayer(const cPlatformLayerInit& init, int maxNrVoices);
		~cSoundPlayer();

        static int getMaxVolume();

        void playSound(int sampleId); // Maximum volume
		void playSound(int sampleId, int vol);

        // Pass the sample ID of the Atreides voice
        void playVoice(int sampleId, int house);

        void playMusic(int sampleId);
        void stopMusic();
        bool isMusicPlaying() const;

		// think about voices, clear voices, etc.
		void think();

	private:
		std::vector<int> voices;
        std::unique_ptr<cSoundData> soundData;
};
