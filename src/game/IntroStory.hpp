#ifndef NND3D_INTROSTORY_HPP
#define NND3D_INTROSTORY_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


std::unique_ptr<GameProcess> create_intro_story_screen(GameContext context);

}   }  // end namespace

#endif
