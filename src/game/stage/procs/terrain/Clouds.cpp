#include "Clouds.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class Clouds : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;

public:
    Clouds(CharacterProcEnv _env,
           EntityManagerCO e_manager,
           const glm::dvec3 & position,
           const glm::dvec3 & size,
           const int texture_index)
    :   env(_env),
        sprite(e_manager.grab_sprite())
    {
        sprite.name = "clouds";
        sprite.set_position(position);
        sprite.set_size(size);
        sprite.texture = texture_index;
        sprite.visible = true;

        // In the past, the level loaded in the source texture coordinates
        // from the level file, with these settings:
        // 318,181,0
        // -- 0,0,318,181
        // 65,76,0
        // -- 0,0,63,76
        // 65,76,0
        // -- 0,0,63,76

        // I don't want to pass in the sources and break encapsulation
        // so for now just set it to this.

        sprite.Aframe[1].set(0, 0, size.x, size.y);
        sprite.frame = 1;
    }

    ~Clouds()
    {
        sprite.proc = nullptr;
    }

    void animate(std::int64_t) override {
    }

    void death_animation() override {
    }

    bool update() override {
        sprite.srcx = sprite.srcx + (fs_speed_factor * 0.5);
        sprite.srcx2 = sprite.srcx2 + (fs_speed_factor * 0.5);
        sprite.Aframe[1].x = sprite.Aframe[1].x + 1;
        sprite.Aframe[1].x2 = sprite.Aframe[1].x2 + 1;
        return true;
    }
};

CharacterProc * create_clouds_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const glm::dvec3 & size,
    const int texture_index)
{
    return new Clouds(env, e_manager, position, size, texture_index);
}

}   }   }
