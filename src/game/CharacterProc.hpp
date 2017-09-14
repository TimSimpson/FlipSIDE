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

////void create_player(PlayerData & player_data, CharacterSprite & sprite,
////	               gsl::span<std::reference_wrapper<CharacterSprite>> & children,
////	               CharacterProcEnv env);


class CharacterProc {
public:
     virtual ~CharacterProc() = default;

	 virtual void animate(CharacterSprite & s, std::int64_t ms) = 0;

     virtual void initialize(CharacterProcEnv env, CharacterSprite & sprite) = 0;

	 virtual void update(
		 CharacterProcEnv env,
		 CharacterSprite & sprite,
		 int j,
		 World & world) = 0;
};


CharacterProc * load_process(const std::string & name);

void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &);

void create_player(
    CharacterProcEnv env, PlayerData & player_data, CharacterSprite & sprite,
    gsl::span<CharacterSprite> & children);

}    }

#endif
