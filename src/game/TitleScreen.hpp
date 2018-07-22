#ifndef NND3D_TITLESCREEN_HPP
#define NND3D_TITLESCREEN_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


std::unique_ptr<GameProcess> create_title_screen(GameContext context);

}   }  // end namespace

#endif
