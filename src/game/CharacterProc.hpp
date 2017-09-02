#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "../view.hpp"

namespace nnd3d { namespace game {

struct CharacterSprite;

class CharacterProc {
public:
     virtual ~CharacterProc() = default;

     virtual void initialize(view::View & view, Sound & sound, const double current_time, Random & random, CharacterSprite & sprite) = 0;
};


CharacterProc * load_process(const std::string & name);

}    }

#endif
