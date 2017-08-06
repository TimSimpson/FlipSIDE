#include "Sound.hpp"

namespace nnd3d {

namespace mix = lp3::mix;

// --------------------------------------------------------------------
// MuteSound impl
// --------------------------------------------------------------------

MuteSound::MuteSound()
{}

void MuteSound::LoadSound(int which, const std::string & soundFile,
	                      std::string soundName) {
    LP3_LOG_VAR(which)
    LP3_LOG_VAR(soundFile)
    LP3_LOG_VAR(soundName)
}

void MuteSound::PlayBgm(const std::string & sound_file) {
    LP3_LOG_VAR(sound_file)
}

void MuteSound::PlayIsoWave(const std::string & soundFile) {
    LP3_LOG_VAR(soundFile)
}

void MuteSound::PlaySound(const std::string & who) {
    LP3_LOG_VAR(who)
}

void MuteSound::PlaySoundLoop(const int which) {
    LP3_LOG_VAR(which)
}

void MuteSound::PlayWave(const std::string & soundFile) {
    LP3_LOG_VAR(soundFile)
}

void MuteSound::garbage_collect() {
}

void MuteSound::silence_sfx() {
}

void MuteSound::StopSound(int which) {
    LP3_LOG_VAR(which)
}

// --------------------------------------------------------------------
// MixSound impl
// --------------------------------------------------------------------

MixSound::MixSound(lp3::core::MediaManager & media_arg)
:   media(media_arg),
    mixer(22050, MIX_DEFAULT_FORMAT, 2, 4096),
    bgm(),
	orphaned_waves(),
	sound_buffers()
{
	constexpr int arbitrary_channel_count = 70;
	Mix_AllocateChannels(16 + arbitrary_channel_count);
	sound_buffers.reserve(arbitrary_channel_count);
}

std::string MixSound::get_file_path(const std::string & file_name) {
    return media.path(file_name);
}

void MixSound::LoadSound(int which, const std::string & soundFile,
                         std::string soundName) {
	LP3_LOG_DEBUG("LoadSound %s", soundFile);

	// TODO: Whoops, looks like I should be deleting any sound_buffers that
	//       already exist with the given `which` index.

	const auto music_path = get_file_path(soundFile);

	SoundBuffer sb;
	sb.name = soundName;
	sb.which_index = which;
	sb.chunk = Mix_LoadWAV(music_path.c_str());

	sound_buffers.emplace_back(std::move(sb));
}

void MixSound::PlayBgm(const std::string & sound_file) {
    if (sound_file.length() == 0) {
        LP3_LOG_DEBUG("PlayBgm [ STOP ]");
        bgm = nullptr;
        return;
    }
    const auto music_path = get_file_path(sound_file);
    Mix_HaltMusic();
    LP3_LOG_DEBUG("PlayBgm %s", music_path);
    Mix_HaltMusic();
    bgm.reset(nullptr);
    bgm.reset(new mix::Music{Mix_LoadMUS(music_path.c_str())});
    bgm->play();
}

void MixSound::PlayIsoWave(const std::string & soundFile) {
	// .... so it's a copy? Huh?
	// Maybe the goal was that if something called this twice, it
	// would stop whatever sound had been playing. <shrug emoji here>
	PlayWave(soundFile);
}

void MixSound::PlaySound(const std::string & who) {
	for (SoundBuffer & sb : sound_buffers) {
		if (sb.name == who) {
			InUseBuffer ow;
			ow.channel = Mix_PlayChannel(-1, sb.chunk, 0);
			if (ow.channel == -1) {
				LP3_LOG_ERROR("PlayChannel error: %s", Mix_GetError());
				LP3_THROW2(lp3::core::Exception, "PlayChannel error");
			}
			orphaned_waves.emplace_back(std::move(ow));
		}
	}
    LP3_LOG_DEBUG("PlaySound %s", who);
}

void MixSound::PlaySoundLoop(const int which) {
    LP3_LOG_DEBUG("PlaySoundLoop %d", which);
	for (SoundBuffer & sb : sound_buffers) {
		if (sb.which_index == which) {
			InUseBuffer ow;
			ow.channel = Mix_PlayChannel(-1, sb.chunk, -1);
			if (ow.channel == -1) {
				LP3_LOG_ERROR("PlayChannel error: %s", Mix_GetError());
				LP3_THROW2(lp3::core::Exception, "PlayChannel error");
			}
			orphaned_waves.emplace_back(std::move(ow));
		}
	}
	LP3_LOG_DEBUG("PlaySoundLoop %s", which);
}

void MixSound::PlayWave(const std::string & soundFile) {
    LP3_LOG_DEBUG("PlayWave %s", soundFile);
	// The old code would play this a zillion times, I think in order to make
	// sure nothing else was playing. Just ignore it here.
	if (soundFile == "nothing.wav") {
		return;
	}
	const auto music_path = get_file_path(soundFile);

	InUseBuffer ow;
	ow.source.reset(new SoundBuffer());

	ow.source->name = soundFile;
	ow.source->chunk = Mix_LoadWAV(music_path.c_str());
	ow.channel = Mix_PlayChannel(-1, ow.source->chunk, 0);
	if (ow.channel == -1) {
		LP3_LOG_ERROR("PlayChannel error: %s", Mix_GetError());
		LP3_THROW2(lp3::core::Exception, "PlayChannel error");
	}

	orphaned_waves.emplace_back(std::move(ow));
}

void MixSound::garbage_collect() {
	orphaned_waves.erase(
		std::remove_if(
			orphaned_waves.begin(), orphaned_waves.end(),
			[](const InUseBuffer & ow) {
				const auto r = Mix_Playing(ow.channel);
				if (0 == r) {
					Mix_HaltChannel(ow.channel);
				}
				return 0 == r;
			}
		),
		orphaned_waves.end());
}

void MixSound::silence_sfx() {
	for (InUseBuffer & ow : orphaned_waves) {
		Mix_HaltChannel(ow.channel);
	}
	orphaned_waves.clear();
}

void MixSound::StopSound(int which) {
    LP3_LOG_VAR(which);
    LP3_LOG_DEBUG("StopSound %d", which);
}



// --------------------------------------------------------------------
// MutableSound impl
// --------------------------------------------------------------------

MutableSound::MutableSound(lp3::core::MediaManager & media)
:   real_sound(media),
	muted_sound(),
	current_sound(&real_sound),
	calls(),
    current_bgm()
{}

void MutableSound::LoadSound(int which, const std::string & soundFile,
                          std::string soundName) {
	if (current_sound == &real_sound) {
		real_sound.LoadSound(which, soundFile, soundName);
	}

    for (std::size_t i = 0; i < this->calls.size(); ++ i) {
        if (this->calls[i].which == which) {
            this->calls[i] = LoadSoundCall{ which, soundFile, soundName };
            return;
        }
    }
	this->calls.emplace_back(LoadSoundCall{ which, soundFile, soundName });
}

void MutableSound::PlayBgm(const std::string & sound_file) {
    current_bgm = sound_file;
	current_sound->PlayBgm(sound_file);
}

void MutableSound::PlayIsoWave(const std::string & soundFile) {
	current_sound->PlayIsoWave(soundFile);
}

void MutableSound::PlaySound(const std::string & who) {
	current_sound->PlaySound(who);
}

void MutableSound::PlaySoundLoop(const int which) {
	current_sound->PlaySoundLoop(which);
}

void MutableSound::PlayWave(const std::string & soundFile) {
	current_sound->PlayWave(soundFile);
}

void MutableSound::garbage_collect() {
	current_sound->garbage_collect();
}

void MutableSound::silence_sfx() {
	real_sound.silence_sfx();
}

void MutableSound::StopSound(int which) {
	current_sound->StopSound(which);
}

void MutableSound::mute() {
	if (current_sound == &real_sound) {
		current_sound = &muted_sound;
	}
}

void MutableSound::unmute() {
	if (current_sound == &muted_sound) {
		for (auto & call : this->calls) {
			this->real_sound.LoadSound(call.which, call.soundFile, call.soundName);
		}
		this->calls.clear();
		this->real_sound.PlayBgm(current_bgm);
		this->current_sound = &real_sound;
	}
}

}   // end namespace
