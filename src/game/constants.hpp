#ifndef NND3D_CONSTANTS_HPP
#define NND3D_CONSTANTS_HPP

#include <glm/glm.hpp>

namespace nnd3d { namespace game {

constexpr std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps

// This used to be a random variable called `sFactor` that could change a
// bunch. Now things are deterministic so it can be gloriously constant.
constexpr double speed_factor = ms_per_update / 1000.0;

const glm::vec4 normal_color{1.0f, 1.0f, 1.0f, 1.0f};

}   }

#endif
