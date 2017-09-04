// Helper functionality for screens.
// All of this was gross stuff living in the original game.cpp that ended up
// being used everywhere.
#ifndef NND3D_BASESCREEN_HPP
#define NND3D_BASESCREEN_HPP
#pragma once

#include "Game.hpp"
#include "../view.hpp"

namespace nnd3d { namespace game {

// 2017: Initializes the game. Port uses a lot of constructors so it misses
// the sheer joy of initializing hundreds of global variables sitting in a
// big static array like the old code did.
void destroyEverything(World & world, view::View & view, Sound & sound, int how = 0);

void flicker(World & world);

void findOrder(World & world);

void create_billboards(World & world, std::vector<view::Billboard> & billboards);

}   }  // end namespace

#endif
