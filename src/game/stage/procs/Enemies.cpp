#include "CinemaProc.hpp"
#include <lp3/sims.hpp>
#include "enemies/BlueStick.hpp"
#include "enemies/Kerbose.hpp"
#include "enemies/PaulRunner.hpp"
#include "enemies/TrashDigger.hpp"

namespace nnd3d { namespace game { namespace proc {


CharacterProc * create_enemy_proc(
    CharacterProcEnv env,
    EntityManagerCO & e_manager,
    const std::string & name,
    const glm::dvec3 & position,
    const int texture_index)
{
    if (name == "Kerbose" || name == "Kirbose") {
        return create_kerbose_proc(env, e_manager, position, texture_index);
    }
    if (name == "bluestick") {
        return create_bluestick_proc(env, e_manager, position, texture_index);
    }
    if (name == "paulrun") {
        return create_paulrunner_proc(env, e_manager, position, texture_index);
    }
    if (name == "tdigger") {
        return create_trashdigger_proc(env, e_manager, position, texture_index);
    }
    return nullptr;
}

}   }   }
