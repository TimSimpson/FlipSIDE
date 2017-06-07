#ifndef NND3D_GAME_HPP
#define NND3D_GAME_HPP
#pragma once

#include <lp3/input.hpp>
#include "Sound.hpp"
#include "World.hpp"
#include "View.hpp"

namespace nnd3d {

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
    Game(lp3::input::Controls & controls, View & view, Sound & sound, 
		 World & world);

    void PlayGame();

private:
	lp3::input::Controls & controls;
    View & view;
    Sound & sound;
    World & world;

    void destroyEverything(int how=0);
};

}   // end namespace

#endif
