#ifndef NND3D_LEGACYGAME_HPP
#define NND3D_LEGACYGAME_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


// ----------------------------------------------------------------------------
// LegacyGame
// ----------------------------------------------------------------------------
//      Binds together logic to play the game.
//      Responsibilities include:
//          * running the process manager class
//          * having some bespoke logic to manage changing "scenes"
//          * handling state transitions in and out of the game process,
//            such as when there's a game over.
// ----------------------------------------------------------------------------
gsl::owner<GameProcess *> create_legacy_screen(
        GameContext context, World && world,
        std::array<boost::optional<std::string>, 3> players,
        const std::string & screen_name);


}   }  // end namespace

#endif
