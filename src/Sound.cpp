#include "Sound.hpp"

namespace nnd3d {

namespace mix = lp3::mix;

Sound::Sound(lp3::core::MediaManager & media)
:   media(media),
    mixer(22050, MIX_DEFAULT_FORMAT, 2, 4096)
{}

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
        return;
    }
    const auto music_path = get_file_path(sound_file);
    LP3_LOG_DEBUG("PlayBgm %s", music_path);
    mix::Music music = Mix_LoadMUS(music_path.c_str());
    music.play();
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
}

void Sound::StopSound(int which) {
    LP3_LOG_DEBUG("StopSound %d", which);
}

}   // end namespace
