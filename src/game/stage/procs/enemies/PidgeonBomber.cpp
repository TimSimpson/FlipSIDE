#include "PidgeonBomber.hpp"
#include <lp3/sims.hpp>

#include "BlueStick.hpp"

namespace nnd3d { namespace game { namespace proc {


class PidgeonBomber : public CharacterProc {
private:
    CharacterProcEnv env;
    EntityManagerCO entity_manager;
    CharacterSprite & sprite;
    const int texture_index;
    bool falling;
public:
    PidgeonBomber(CharacterProcEnv _env,
                  EntityManagerCO e_manager,
                  const glm::dvec3 & position,
                  const int texture_index_arg)
    :   env(_env),
        entity_manager(e_manager),
        sprite(e_manager.grab_sprite()),
        texture_index(texture_index_arg),
        falling(false)
    {
        sprite.proc = this;
        sprite.name = "pigeonbomber";
        sprite.set_position(position);
        sprite.texture = texture_index;

        env.context.view.load_animation_file(sprite.Aframe, "pigeon.ani");

        sprite.visible = true;
        sprite.mph = 1;
        sprite.seekx = -50;
        sprite.seeky = sprite.y;
        sprite.wide = 40;
        sprite.high = 30;
        sprite.hp = 1;
        sprite.z = 80;
        sprite.length = 30;
        sprite.frame = 1;
        sprite.deathType = "falling";
        sprite.kind = Kind::enemy_weak_to_jumping;
    }

    ~PidgeonBomber() {
        sprite.proc = nullptr;
    }

    void animate(std::int64_t) override {
        ++ sprite.frame;
        if (sprite.frame > 2) {
            sprite.frame = 1;
        }
     }

    void death_animation() override {
        falling = true;
        sprite.kind = Kind::neutral;
    }

    bool update() override {
        if (!falling) {
            sprite.z = sprite.z + fs_speed_factor;
            //sprite.frame = sprite.frame + 1
            //if (sprite.frame > 2) then sprite.frame = 1

            seek(sprite);
            if (sprite.x < (sprite.wide * -1)) {
                sprite.x = env.camera.boundary().x;
            }

            if (sprite.miscTime < env.current_time) {
                const auto target
                    = entity_manager.find_closest_player(sprite);
                env.processes.spawn(
                    create_bluestick_proc(
                        env,
                        entity_manager,
                        sprite.get_position(),
                        texture_index)
                );
                sprite.miscTime = env.current_time + 2;
            }
            return true;
        } else {
            sprite.z = sprite.z - fs_speed_factor;
            if (sprite.z < 1) {
                sprite.z = 1;
                sprite.name = "deceased";
                sprite.visible = false;
                sprite.trueVisible = 2;
                sprite.flickerTime = 0;
            }
            sprite.frame = sprite.frame + 1;
            if (sprite.frame > 4) { sprite.frame = 3; }
            return false;
        }
    }
};

CharacterProc * create_pidgeonbomber_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new PidgeonBomber(env, e_manager, position, texture_index);
}

}   }   }
