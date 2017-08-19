#ifndef NND3D_GAMECONTEXT_HPP
#define NND3D_GAMECONTEXT_HPP
#pragma once

#include "../Random.hpp"
#include "../Sound.hpp"
#include "../View.hpp"

namespace nnd3d { namespace game {


// The environment a game is running in; refers to physical systems
// outside the game logics primary concern which it may still need to
// prod and poke.
struct Env {
    view::View & view;
    Sound & sound;
    Random & random;

    Env(view::View & view_arg,
                Sound & sound_arg,
                Random & random_arg);
};

}   }

#endif
