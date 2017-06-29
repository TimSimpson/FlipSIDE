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
    // For now, do nothing.
}

void Sound::PlayIsoWave(const std::string & soundFile) {
    // For now, do nothing.
}

void Sound::PlaySound(const std::string & who) {
    // For now, do nothing.
}

void Sound::PlaySoundLoop(const int which) {
    // For now, do nothing.
}

void Sound::PlayWave(const std::string & soundFile) {
    // For now, do nothing.
}

void Sound::garbage_collect() {
    // For now, do nothing.
}

void Sound::silence_sfx() {
    // For now, do nothing.
}

void Sound::StopSound(int which) {
    // For now, do nothing.
}

}   // end namespace
