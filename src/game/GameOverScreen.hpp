#ifndef NND3D_GAMEOVERSCREEN_HPP
#define NND3D_GAMEOVERSCREEN_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


gsl::owner<GameProcess *> create_gameover_screen(GameContext context);

}   }  // end namespace

#endif
