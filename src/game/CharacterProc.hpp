#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "Env.hpp"

namespace nnd3d { namespace game {

struct CharacterSprite;

class CharacterProc {
public:
     virtual ~CharacterProc() = default;

	 virtual void animate(CharacterSprite & sprite) = 0;

     virtual void initialize(Env & env, const double current_time, 
		                     CharacterSprite & sprite) = 0;	 
};


CharacterProc * load_process(const std::string & name);

}    }

#endif
