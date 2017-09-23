#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "EntityManager.hpp"
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
    Camera & camera;
};

////void create_player(PlayerData & player_data, CharacterSprite & sprite,
////	               gsl::span<std::reference_wrapper<CharacterSprite>> & children,
////	               CharacterProcEnv env);


class CharacterProc {
public:
     virtual ~CharacterProc() = default;

	 // TODO: The plethora of virtual methods with default impls below was to
	 // support the legacy procs, which needed a gross, large interface.
	 // In the future delete them.
	 virtual void animate(std::int64_t ms) {
	 }

	 virtual void death_animation() {
		 throw std::logic_error("The method or operation is not implemented.");
	 }

	 virtual void initialize() {
		 throw std::logic_error("The method or operation is not implemented.");
	 }
     
	 // Create a child process (think bullets)
	 virtual CharacterProc * spawn(CharacterSprite & sprite, const std::string & name) {
		 throw std::logic_error("The method or operation is not implemented.");
	 }

	 virtual bool update() = 0;
};

// void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &);

// void create_player(
//     CharacterProcEnv env, PlayerData & player_data, CharacterSprite & sprite,
//     gsl::span<CharacterSprite> & children);


class CharacterProcManager {
public:
    CharacterProcManager();

    ~CharacterProcManager();

    CharacterProcManager(CharacterProcManager && rhs);

    void add_process(gsl::owner<CharacterProc *> process);

	void animate(std::int64_t ms);

    void clear();

    void update();
private:
    std::vector<gsl::owner<CharacterProc *>> procs;
};


// Uses a bunch of hacks resembling the old code to figure out how
gsl::owner<CharacterProc *> legacy_add_process(
	CharacterProcEnv & env, World & world, EntityManager & em,
	int j, CharacterSprite & s, const std::string & name);

}    }

#endif
