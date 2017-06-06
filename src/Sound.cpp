#include "Sound.hpp"

namespace nnd3d {

void Sound::LoadSound(int which, const std::string & soundFile,
                      std::string soundName) {
    LP3_LOG_DEBUG("LoadSound", soundFile);
}

void Sound::PlayBgm(const std::string & soundFile) {
    LP3_LOG_DEBUG("PlayBgm %s", soundFile);
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
