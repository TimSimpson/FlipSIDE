#include "Env.hpp"


namespace nnd3d { namespace game {

Env::Env(view::View & view_arg, Sound & sound_arg, Random & random_arg)
:   view(view_arg),
    sound(sound_arg),
    random(random_arg)
{}

}   }
