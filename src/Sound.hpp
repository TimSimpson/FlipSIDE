#ifndef NND3D_SOUND_HPP
#define NND3D_SOUND_HPP
#pragma once

#include <lp3/core.hpp>

namespace nnd3d {

// Handles everything involving sound.
class Sound
{
public:
    void LoadSound(int which, const string & soundFile, string soundName = "");

    void PlayBgm(const string & soundFile);

    void PlayIsoWave(const string & soundFile);}

    void PlaySound(const string & who);

    void PlaySoundLoop(int which);

    void PlayWave(const string & soundFile);

    void StopSound(int which);
};

}   // end namespace
