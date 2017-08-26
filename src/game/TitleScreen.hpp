#ifndef NND3D_TITLESCREEN_HPP
#define NND3D_TITLESCREEN_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


GameProcess * create_title_screen(GameProcessSpace & space,
                                  view::View & view, Sound & sound, Vb & vb, 
	                              Random & random, World & world);

}   }  // end namespace

#endif
