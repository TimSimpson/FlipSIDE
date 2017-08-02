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
	virtual ~Sound() = default;

    virtual void LoadSound(int which, const std::string & soundFile,
                           std::string soundName = "") = 0;

	virtual void PlayBgm(const std::string & soundFile) = 0;

	virtual void PlayIsoWave(const std::string & soundFile) = 0;

	virtual void PlaySound(const std::string & who) = 0;

	virtual void PlaySoundLoop(const int which) = 0;

	virtual void PlayWave(const std::string & soundFile) = 0;

	virtual void StopSound(int which) = 0;

	// Must be called periodically to clean stuff up.
	virtual void garbage_collect() = 0;

	// This is a new part of the interface. In the old code, it would call
	// PlayWave with `nothing.wav` over and over. That was too crazy to be
	// worth emulating so this function just mutes everything but the bgm.
	virtual void silence_sfx() = 0;
};

class MuteSound : public Sound
{
public:
	MuteSound();

	void LoadSound(int which, const std::string & soundFile,
		           std::string soundName = "") override;

	void PlayBgm(const std::string & soundFile) override;

	void PlayIsoWave(const std::string & soundFile) override;

	void PlaySound(const std::string & who) override;

	void PlaySoundLoop(const int which) override;

	void PlayWave(const std::string & soundFile) override;

	void StopSound(int which) override;

	void garbage_collect() override;

	void silence_sfx() override;	
};

class MixSound : public Sound
{
public:
	MixSound(lp3::core::MediaManager & media);

	void LoadSound(int which, const std::string & soundFile,
		           std::string soundName = "") override;

	void PlayBgm(const std::string & soundFile) override;

	void PlayIsoWave(const std::string & soundFile) override;

	void PlaySound(const std::string & who) override;

	void PlaySoundLoop(const int which) override;

	void PlayWave(const std::string & soundFile) override;

	void StopSound(int which) override;

	void garbage_collect() override;

	void silence_sfx() override;

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

class MutableSound : public Sound
{
public:
	MutableSound(lp3::core::MediaManager & media);

	void LoadSound(int which, const std::string & soundFile,
		           std::string soundName = "") override;

	void PlayBgm(const std::string & soundFile) override;

	void PlayIsoWave(const std::string & soundFile) override;

	void PlaySound(const std::string & who) override;

	void PlaySoundLoop(const int which) override;

	void PlayWave(const std::string & soundFile) override;

	void StopSound(int which) override;

	void garbage_collect() override;

	void silence_sfx() override;

	void mute();

	void unmute();
private:
	struct LoadSoundCall {
		int which;
		std::string soundFile;
		std::string soundName;
	};

	MixSound real_sound;
	MuteSound muted_sound;
	Sound * current_sound;
	
	std::vector<LoadSoundCall> calls;

	std::string current_bgm;
};

}   // end namespace

#endif
