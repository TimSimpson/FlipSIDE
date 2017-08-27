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
		GameProcessSpace & space,
		view::View & view, Sound & sound, Vb & vb, World & world,
		std::array<bool, 3> keys_pressed);
////
////class LegacyGame : public GameProcess
////{
////public:
////    LegacyGame(GameProcessSpace & space,
////		       view::View & view, Sound & sound, Vb & vb, Random & random, World & world);
////
////    ~LegacyGame();
////
////    void handle_input(const input::Event & even) override;
////
////    void update() override;
////
////private:
////    class GameImpl;
////    gsl::owner<GameImpl *> impl;
////};

}   }  // end namespace

#endif
