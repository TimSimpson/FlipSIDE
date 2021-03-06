#include "CharacterProcFactory.hpp"
#include <lp3/sims.hpp>
#include "enemies/Arby.hpp"
#include "enemies/BlueStick.hpp"
#include "enemies/Kerbose.hpp"
#include "enemies/PaulRunner.hpp"
#include "enemies/PidgeonBomber.hpp"
#include "enemies/TrashDigger.hpp"
#include "terrain/Clouds.hpp"
#include "terrain/GreenSpring.hpp"

namespace nnd3d { namespace game { namespace proc {


CharacterProc * create_character_proc(
    CharacterProcEnv env,
    EntityManagerCO & e_manager,
    const std::string & name,
    const glm::dvec3 & position,
    const glm::dvec3 & size,
    const int texture_index)
{
    if (name == "arby" || name == "goomba") {
        return create_arby_proc(env, e_manager, position, texture_index);
    }
    if (name == "Kerbose" || name == "Kirbose") {
        return create_kerbose_proc(env, e_manager, position, texture_index);
    }
    if (name == "bluestick") {
        return create_bluestick_proc(env, e_manager, position, texture_index);
    }
    if (name == "paulrun") {
        return create_paulrunner_proc(env, e_manager, position, texture_index);
    }
    if (name == "pigeonbomber") {
        return create_pidgeonbomber_proc(env, e_manager, position, texture_index);
    }
    if (name == "tdigger") {
        return create_trashdigger_proc(env, e_manager, position, texture_index);
    }

    if (name == "greenspring") {
        return create_greenspring_proc(env, e_manager, position, texture_index);
    }
    if (name == "clouds") {
        return create_clouds_proc(
            env, e_manager, position, size, texture_index);
    }
    return nullptr;
}

}   }   }
