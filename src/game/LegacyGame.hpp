#ifndef NND3D_LEGACYGAME_HPP
#define NND3D_LEGACYGAME_HPP
#pragma once

#include "Game.hpp"

namespace nnd3d { namespace game {


// --------------------------------------------------------------------
// LegacyGame
// --------------------------------------------------------------------
//      For now this is where the bulk of the code from NND3d2.bas
//      will live. The state from that mammoth module is represented
//      by the World struct.
// --------------------------------------------------------------------
gsl::owner<GameProcess *> create_legacy_screen(
		view::View & view, Sound & sound, Vb & vb, World && world,
		std::array<boost::optional<std::string>, 3> players);


}   }  // end namespace

#endif
