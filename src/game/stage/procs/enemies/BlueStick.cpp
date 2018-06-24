#include "BlueStick.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class BlueStick : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    lp3::sims::CoroutineState state;
    bool dying;

public:
    BlueStick(CharacterProcEnv _env, EntityManagerCO e_manager,
            const glm::dvec3 & position,
            const int texture_index)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
        state(),
        dying(false)
    {
        sprite.name = "bluestick";
        sprite.set_position(position);
        sprite.texture = texture_index;
        sprite.hp = 1;
        sprite.kind = Kind::enemy_weak_to_jumping;
        sprite.wide = 10;
        sprite.high = 17;
        sprite.length = 10;
        sprite.Aframe[1].set(1, 173, 10, 190);
        sprite.Aframe[2].set(13, 173, 23, 190);
        sprite.Aframe[3].set(23, 174, 33, 190);
        sprite.deathType = "bs death";
        sprite.soundFile = "Stick Ouch";
        env.context.sound.PlaySound("Stick Awaken");
        sprite.frame = 1;
        unstretch(sprite);
        sprite.proc = this;
    }

    ~BlueStick()
    {
        sprite.proc = nullptr;
    }

    void animate(std::int64_t) override {
        if (dying) {
            sprite.frame = 3;
        } else {
            sprite.frame = sprite.frame == 2 ? 1 : 2;
        }
    }

    void death_animation() override {
        sprite.visible = true;

        sprite.kind = Kind::neutral;
        sprite.frame = 3;

        sprite.miscTime = env.current_time + 3;

        env.context.sound.PlaySound("stick die");
        sprite.name = "";
        dying = true;
    }

    void wiggle() {
        const auto move_x = (int)(env.random.next() * 2) + 1;
        if (move_x == 1) { sprite.x += fs_speed_factor; }
        if (move_x == 2) { sprite.x -= fs_speed_factor; }
        const auto move_y = (int)(env.random.next() * 2) + 1;
        if (move_y == 1) { sprite.y = sprite.y + fs_speed_factor; }
        if (move_y == 2) { sprite.y = sprite.y - fs_speed_factor; }
        const auto jump = (int)(env.random.next() * 20) + 1;
        if (jump == 1) {
            if (sprite.z == 0) {
                sprite.start_jump();
            }
        }
    }

    bool update() override {
        LP3_COROUTINE_BEGIN(state)
            while (!dying) {
                wiggle();
                LP3_YIELD(true);
            }

            while (sprite.miscTime >= env.current_time) {
                LP3_YIELD(true);
            }

            sprite.visible = false;
            sprite.trueVisible = 2;
            sprite.flickerTime = env.current_time + 1;
            while (sprite.miscTime + 1 >= env.current_time) {
                LP3_YIELD(true);
            }
            kill(sprite);
        LP3_COROUTINE_END()
        return false;
    }
};

CharacterProc * create_bluestick_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new BlueStick(env, e_manager, position, texture_index);
}

}   }   }
