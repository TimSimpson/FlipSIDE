#include "Sound.hpp"

namespace nnd3d {

namespace mix = lp3::mix;

Sound::Sound(lp3::core::MediaManager & media)
:   media(media),
    mixer(22050, MIX_DEFAULT_FORMAT, 2, 4096),
    bgm(),
	orphaned_waves(),
	sound_buffers()
{
	Mix_AllocateChannels(16 + 30);  // According to the docs, this never fails.
	sound_buffers.reserve(30);
}

std::string Sound::get_file_path(const std::string & file_name) {
    return media.path(file_name);
}

void Sound::LoadSound(int which, const std::string & soundFile,
                      std::string soundName) {
	LP3_LOG_DEBUG("LoadSound %s", soundFile);

	const auto music_path = get_file_path(soundFile);

	SoundBuffer sb;
	sb.name = soundName;
	sb.which_index = which;
	sb.chunk = Mix_LoadWAV(music_path.c_str());	

	sound_buffers.emplace_back(std::move(sb));    
}

void Sound::PlayBgm(const std::string & sound_file) {
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

void Sound::PlayIsoWave(const std::string & soundFile) {
	// .... so it's a copy? Huh?
	// Maybe the goal was that if something called this twice, it
	// would stop whatever sound had been playing. <shrug emoji here>
	PlayWave(soundFile);
}

void Sound::PlaySound(const std::string & who) {
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

void Sound::PlaySoundLoop(const int which) {
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

void Sound::PlayWave(const std::string & soundFile) {
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

void Sound::garbage_collect() {
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

void Sound::silence_sfx() {
	for (InUseBuffer & ow : orphaned_waves) {
		Mix_HaltChannel(ow.channel);
	}
	orphaned_waves.clear();
}

void Sound::StopSound(int which) {
    LP3_LOG_DEBUG("StopSound %d", which);
}

}   // end namespace
