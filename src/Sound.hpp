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

    void PlaySoundLoop(const int which);

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
    std::unique_ptr<lp3::mix::Music> bgm;    

	// Represents a sound buffer in the old code.
	// I've combined a few variables which used to be part of independent
	// arrays.
	struct SoundBuffer {
		lp3::mix::Chunk chunk;	// SDL resource
		std::string name;		// made up name code uses to refer to this		
		int which_index;        // used to map to the old code's `which` arg
	};

	struct InUseBuffer {
		// For orphaned buffers (see below) the buffer will be attached to 
		// this and cleaned up later.
		std::unique_ptr<SoundBuffer> source;
		int channel;            // which channel SDL is using for it
	};

	// The old code would play wave files by name, and never managed the
	// resulting resources. Here we need to make sure we clean up the
	// chunk needed to play the wave, but only after it's done playing.
	// I think the old game had an array of 15 of these, but here I'll
	// just grow it dynamically under the assumption the related 
	// functions don't get called too often (... or do they?!!)
	std::vector<InUseBuffer> orphaned_waves;

	// There were also 20 sound buffers that could be named (the code declared
	// 30 but due to a bug I think only 20 got used).
	std::vector<SoundBuffer> sound_buffers;

	std::string get_file_path(const std::string & file_name);

	//SoundBuffer && load_buffer(const std::string & sound_file);
};

}   // end namespace

#endif
