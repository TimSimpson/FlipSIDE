#include "Sound.hpp"

namespace nnd3d {

namespace mix = lp3::mix;

Sound::Sound(lp3::core::MediaManager & media)
:   media(media),
    mixer(22050, MIX_DEFAULT_FORMAT, 2, 4096),
    bgm(),
	chunks()
{
	Mix_AllocateChannels(16);  // According to the docs, this never fails.
}

std::string Sound::get_file_path(const std::string & file_name) {
    std::string path = str(boost::format("FlipSIDE/%s") % file_name);
    return media.path(path);
}

void Sound::LoadSound(int which, const std::string & soundFile,
                      std::string soundName) {
    LP3_LOG_DEBUG("LoadSound", soundFile);
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
    bgm.reset(new mix::Music{Mix_LoadMUS(music_path.c_str())});
    bgm->play();
}

void Sound::PlayIsoWave(const std::string & soundFile) {
    LP3_LOG_DEBUG("PlayIsoWave %s", soundFile);
}

void Sound::PlaySound(const std::string & who) {
    LP3_LOG_DEBUG("PlaySound %s", who);
}

void Sound::PlaySoundLoop(int which) {
    LP3_LOG_DEBUG("PlaySoundLoop %d", which);
}

void Sound::PlayWave(const std::string & soundFile) {
    LP3_LOG_DEBUG("PlayWave %s", soundFile);
	// The old code would play this a zillion times, I think in order to make
	// sure nothing else was playing. Just ignore it here.
	if (soundFile == "nothing.wav") {
		return;
	}
	const auto music_path = get_file_path(soundFile);

	OrphanWave ow;
	ow.name = soundFile;
	ow.chunk = Mix_LoadWAV(music_path.c_str());
	ow.channel = Mix_PlayChannel(-1, ow.chunk, 0);
	if (ow.channel == -1) {
		LP3_LOG_ERROR("PlayChannel error: %s", Mix_GetError());
		LP3_THROW2(lp3::core::Exception, "PlayChannel error");
	}

	chunks.emplace_back(std::move(ow));
}

void Sound::garbage_collect() {
	chunks.erase(
		std::remove_if(
			chunks.begin(), chunks.end(),
			[](const OrphanWave & ow) {
				const auto r = Mix_Playing(ow.channel);
				if (0 == r) {
					Mix_HaltChannel(ow.channel);
				}
				return 0 == r;
			}
		),
		chunks.end());
}

void Sound::silence_sfx() {
	for (OrphanWave & ow : chunks) {
		Mix_HaltChannel(ow.channel);
	}
	chunks.clear();
}

void Sound::StopSound(int which) {
    LP3_LOG_DEBUG("StopSound %d", which);
}

}   // end namespace
