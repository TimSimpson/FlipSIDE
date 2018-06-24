#include "PaulRunner.hpp"
#include <lp3/sims.hpp>

#include "BlueStick.hpp"

namespace nnd3d { namespace game { namespace proc {


class PaulRunner : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    const int texture_index;
    bool dying;
    enum class Skin {
        PaulRunner,
        Putulo
    } skin;
    lp3::sims::CoroutineState state;

public:
    PaulRunner(CharacterProcEnv _env,
               EntityManagerCO e_manager,
               const glm::dvec3 & position,
               const int texture_index_arg)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
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
        // sprite.visible = true;

        // sprite.kind = Kind::neutral;
        // sprite.frame = 3;

        // sprite.miscTime = env.current_time + 3;
        // if (skin == Skin::PaulRunner) {
        //     env.context.sound.PlaySound("PaulRunner Die");
        // } else if (skin == Skin::Putulo) {
        //     env.context.sound.PlaySound("putulodie");
        // }
        // sprite.name = "";
        dying = true;
        env.context.sound.PlaySound("Paul Shrink");
    }

    void move() {
        LP3_COROUTINE_BEGIN(state)
            while(true) {
                sprite.seekx = 0;
                sprite.dir = "";
                while (sprite.seekx <= 100) {
                    sprite.x = sprite.x + fs_speed_factor;
                    sprite.seekx = sprite.seekx + fs_speed_factor;
                    LP3_YIELD();
                }
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
        // LP3_LOG_INFO("paul (%d, %d), (%d, %d), %s, %d, %d, frame=(%d, %d)-(%d, %d)", sprite.x, sprite.y, sprite.wide, sprite.high, sprite.visible ? "TRUE": "false", sprite.texture, sprite.zOrder,
        //     sprite.srcx, sprite.srcy, sprite.srcx2, sprite.srcy2);

        if (dying) {
            EntityManager e_manager(sprite);
            // change to blue stick
            env.processes.spawn(
                create_bluestick_proc(
                    env,
                    EntityManagerCO{e_manager},
                    sprite.get_position(),
                    texture_index)
            );
            return false;
        } else {
            move();
            LP3_LOG_INFO("paul2 %d, %d, %d, %d, %s", sprite.x, sprite.y, sprite.wide, sprite.high, sprite.visible);

            if (sprite.seekx >= 50 && sprite.dir != "done") {
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
