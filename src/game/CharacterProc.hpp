#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include <functional>
#include "Camera.hpp"
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

     virtual void update(Env & env, const double current_time,
                         CharacterSprite & sprite,
						 Camera & camera,
						 PlayerData * player_data,
                         gsl::span<CharacterSprite> & children,
						 World & world) = 0;
};

void load_process(CharacterSprite & s, const std::string & name);

void load_process_init(CharacterSprite & s, const std::string & name, Env & env,
                       const double current_time);

void create_player(PlayerData & player_data, CharacterSprite & sprite,
	               gsl::span<CharacterSprite> & children,
                   Env & env);

}    }

#endif
