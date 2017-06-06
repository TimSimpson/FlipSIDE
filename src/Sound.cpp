#include "Sound.hpp"

namespace nnd3d {

void Sound::LoadSound(int which, const string & soundFile,
                      string soundName = "") {
    LP3_LOG_DEBUG("LoadSound", soundFile);
}

void Sound::PlayBgm(const string & soundFile) {
    LP3_LOG_DEBUG("PlayBgm %s", soundFile);
}

void Sound::PlayIsoWave(const string & soundFile) {
    LP3_LOG_DEBUG("PlayIsoWave %s", soundFile);
}

void Sound::PlaySound(const string & who) {
    LP3_LOG_DEBUG("PlaySound %s", who);
}

void Sound::PlaySoundLoop(int which) {
    LP3_LOG_DEBUG("PlaySoundLoop %d", which);
}

void Sound::PlayWave(const string & soundFile) {
    LP3_LOG_DEBUG("PlayWave %s", soundFile);
}

void Sound::StopSound(int which) {
    LP3_LOG_DEBUG("StopSound %d", which);
}

}   // end namespace
