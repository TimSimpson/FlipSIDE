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

private:
    lp3::core::MediaManager & media;
    lp3::mix::Mixer mixer;

    std::string get_file_path(const std::string & file_name);
};

}   // end namespace

#endif
