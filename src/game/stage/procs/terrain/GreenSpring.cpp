#include "GreenSpring.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class GreenSpring : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;

public:
    GreenSpring(CharacterProcEnv _env, EntityManagerCO e_manager,
            const glm::dvec3 & position,
            const int texture_index)
    :   env(_env),
        sprite(e_manager.grab_sprite())
    {
        sprite.name = "greenspring";
        sprite.set_position(position);
        sprite.texture = texture_index;
        sprite.visible = true;

        env.context.view.load_animation_file(sprite.Aframe, "greenspring.ani");
        sprite.frame = 1;
        sprite.mode = "off";
        sprite.hp = 1;
        sprite.deathType = "greenspring";
        sprite.time = 0;
        sprite.kind = Kind::trampoline;
        sprite.bounce_factor = 1.5;
        sprite.length = 10;
        sprite.wide = 24 * 2;
        sprite.high = 35 * 2;
    }

    ~GreenSpring()
    {
        sprite.proc = nullptr;
    }

    void animate(std::int64_t) override {
    }

    void death_animation() override {
    }

    bool update() override {
        if (sprite.mode == "bounce") {
            sprite.frame = sprite.frame + 1;
            if (sprite.frame > 5) {
                sprite.frame = 2;
            }
            sprite.time += fs_s_per_update;
            if (sprite.miscTime < sprite.time) {
                sprite.mode = "";
                sprite.frame = 1;
            }
        }
        return true;
    }
};

CharacterProc * create_greenspring_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new GreenSpring(env, e_manager, position, texture_index);
}

}   }   }
