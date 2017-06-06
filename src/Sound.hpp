#ifndef NND3D_SOUND_HPP
#define NND3D_SOUND_HPP
#pragma once

#include <lp3/core.hpp>

namespace nnd3d {

// Handles everything involving sound.
class Sound
{
public:
    void LoadSound(int which, const std::string & soundFile,
                   std::string soundName = "");

    void PlayBgm(const std::string & soundFile);

    void PlayIsoWave(const std::string & soundFile);

    void PlaySound(const std::string & who);

    void PlaySoundLoop(int which);

    void PlayWave(const std::string & soundFile);

    void StopSound(int which);
};

}   // end namespace

#endif
