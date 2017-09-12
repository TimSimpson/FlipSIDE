#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "Game.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "../view.hpp"

namespace nnd3d { namespace game {

struct CharacterSprite;

struct CharacterProcEnv {
	GameContext context;
	Random & random;
	const double & current_time;	
};

class CharacterProc {
public:
     virtual ~CharacterProc() = default;

	 virtual void animate(CharacterSprite & s, std::int64_t ms) = 0;

     virtual void initialize(CharacterProcEnv env, CharacterSprite & sprite) = 0;

	 //virtual void update(Env &)
};


CharacterProc * load_process(const std::string & name);

}    }

#endif
