#include "PaulRunner.hpp"
#include <lp3/sims.hpp>

#include "BlueStick.hpp"
#include "RedBullet.hpp"

namespace nnd3d { namespace game { namespace proc {


class PaulRunner : public CharacterProc {
private:
    CharacterProcEnv env;
    EntityManagerCO e_manager;
    CharacterSprite & sprite;
    CharacterSprite & bullet_sprite;
    const int texture_index;
    bool dying;
    lp3::sims::CoroutineState state;

public:
    PaulRunner(CharacterProcEnv _env,
               EntityManagerCO e_manager_arg,
               const glm::dvec3 & position,
               const int texture_index_arg)
    :   env(_env),
        e_manager(e_manager_arg),
        sprite(e_manager.grab_sprite()),
        bullet_sprite(e_manager.grab_sprite()),
        texture_index(texture_index_arg),
        dying(false),
        state()
    {
        sprite.name = "paulrun";
        sprite.set_position(position);
        sprite.texture = texture_index;

        env.context.view.load_animation_file(sprite.Aframe, "paulrun.ani");
        sprite.frame = 1;
        sprite.kind = Kind::enemy_weak_to_jumping;
        sprite.invTime = 1;
        sprite.hp = 2;
        sprite.soundFile = "Paul Ouch";
        sprite.length = 20;

        sprite.wide = 32;
        sprite.high = 64;
        sprite.visible = true;

        sprite.seekx = 0; // I guess?
        sprite.proc = this;
    }

    void animate(std::int64_t) override {
        ++ sprite.frame;
        if (sprite.frame > 2) {
            sprite.frame = 1;
        }
     }

    void death_animation() override {
        dying = true;
        env.context.sound.PlaySound("Paul Shrink");
    }

    void move() {
        LP3_COROUTINE_BEGIN(state)
            while(true) {
                sprite.reverse = false;
                sprite.seekx = 0;
                sprite.dir = "";
                while (sprite.seekx <= 100) {
                    sprite.x = sprite.x + fs_speed_factor;
                    sprite.seekx = sprite.seekx + fs_speed_factor;
                    LP3_YIELD();
                }
                sprite.reverse = true;
                sprite.seekx = 0;
                sprite.dir = "";
                while (sprite.seekx <= 100) {
                    sprite.x = sprite.x - fs_speed_factor;
                    sprite.seekx = sprite.seekx + fs_speed_factor;
                    LP3_YIELD();
                }
            }
        LP3_COROUTINE_END()
    }

    bool update() override {
        if (dying) {
            EntityManager e_manager2(sprite);
            // change to blue stick
            env.processes.spawn(
                create_bluestick_proc(
                    env,
                    EntityManagerCO{e_manager2},
                    sprite.get_position(),
                    texture_index)
            );
            return false;
        } else {
            move();
            if (bullet_sprite.proc == nullptr &&
                sprite.seekx >= 50 && sprite.dir != "done")
            {
                const auto target = e_manager.find_closest_player(sprite);
                if (target) {
                    EntityManager e_manager2(bullet_sprite);
                    // change to blue stick
                    env.processes.spawn(
                        create_redbullet_proc(
                            env,
                            EntityManagerCO{e_manager2},
                            sprite.get_position(),
                            target->get_position(),
                            texture_index)
                    );
                    LP3_LOG_INFO("launching bullet at %d, %d", target->get_position().x, target->get_position().y);
                    sprite.dir = "done";
                }
                // TODO: ADD THIS BACK!
                // const auto target = world.find_closest_player(s);
                // if (target) {
                //     this->shoot(s, "paulbullet",
                //                 target->x,
                //                 target->y);
                // }
                // s.dir = "done";
            }
            return true;
        }
    }
};

CharacterProc * create_paulrunner_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new PaulRunner(env, e_manager, position, texture_index);
}

}   }   }
