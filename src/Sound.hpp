#ifndef NND3D_SOUND_HPP
#define NND3D_SOUND_HPP
#pragma once

#include <lp3/core.hpp>
#include <lp3/mix.hpp>

namespace nnd3d {

// Handles everything involving sound.
class Sound
{
public:
    Sound(lp3::core::MediaManager & media);

    void LoadSound(int which, const std::string & soundFile,
                   std::string soundName = "");

    void PlayBgm(const std::string & soundFile);

    void PlayIsoWave(const std::string & soundFile);

    void PlaySound(const std::string & who);

    void PlaySoundLoop(int which);

    void PlayWave(const std::string & soundFile);

    void StopSound(int which);

	// Must be called periodically to clean stuff up.
	void garbage_collect();

	// This is a new part of the interface. In the old code, it would call
	// PlayWave with `nothing.wav` over and over. That was too crazy to be
	// worth emulating so this function just mutes everything but the bgm.
	void silence_sfx();

private:
    lp3::core::MediaManager & media;
    lp3::mix::Mixer mixer;

    std::string get_file_path(const std::string & file_name);

	// The old code would play wave files by name, and never managed the
	// resulting resources. Here we need to make sure we clean up the
	// chunk needed to play the wave, but only after it's done playing.
	struct OrphanWave {
		lp3::mix::Chunk chunk;
		std::string name;
		int channel;  // which channel SDL is using for it
	};

	std::vector<OrphanWave> chunks;
};

}   // end namespace

#endif
