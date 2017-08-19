#ifndef NND3D_GAME_HPP
#define NND3D_GAME_HPP
#pragma once

#include "../Input.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "World.hpp"
#include "../Vb.hpp"
#include "../View.hpp"

namespace nnd3d { namespace game {

namespace core = lp3::core;
namespace gfx = lp3::gfx;

// --------------------------------------------------------------------
// Game
// --------------------------------------------------------------------
//      For now this is where the bulk of the code from NND3d2.bas
//      will live. The state from that mammoth module is represented
//      by the World struct.
// --------------------------------------------------------------------
class Game
{
public:
    Game(view::View & view, Sound & sound, Vb & vb, Random & random, World & world);

	~Game();

    void handle_input(const input::Event & even);

    void PlayGame();

    void TimedEvents();

private:
    class GameImpl;
	gsl::owner<GameImpl *> impl;
};

}   }  // end namespace

#endif
