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

// Makes spries flicker.
void flicker(World & world);

// Takes the data living in "Sprites" inside of the World struct and translates
// that into "Billboards".
// NOTE: this is part of an attempt to get the "View" and game logic to speak
//       in independent terms.
//       Giving the view it's own concepts makes non game screens easier to
//       write.
void create_billboards(World & world,
                       const glm::ivec2 & resolution,
                       std::vector<view::Billboard> & billboards);

void set_time_stuff(World & world);

}   }  // end namespace

#endif
