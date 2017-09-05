#ifndef NND3D_SELECTSCREEN_HPP
#define NND3D_SELECTSCREEN_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


GameProcess * create_select_screen(view::View & view, Sound & sound, Vb & vb,
                                   World & world,
	                               std::array<bool, 3> keys_pressed);

}   }  // end namespace

#endif
