#include "Kerbose.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class Kerbose : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    bool dying;
    enum class Skin {
        Kerbose,
        Putulo
    } skin;
    lp3::sims::CoroutineState state;

public:
    Kerbose(CharacterProcEnv _env, EntityManagerCO & e_manager,
            const glm::dvec3 & position,
            const int texture_index)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
        dying(false),
        state()
    {
        sprite.name = "Kerbose";
        sprite.set_position(position);
        sprite.texture = texture_index;
        sprite.wide = 21;
        sprite.high = 19;
        sprite.visible = true;
        sprite.frame = 1;
        sprite.kind = Kind::enemy_weak_to_jumping;
        sprite.length = 10;
        sprite.deathType = "Kerbose Death";
        sprite.invTime = 1;
        sprite.hp = 3;

        sprite.soundFile = "Kerbose Ouch";
        sprite.jump_strength = 25;
        sprite.proc = this;
        const int kerbose_skin = (int) (env.random.next() * 2.0 + 1);
        if (kerbose_skin == 1) {
            skin = Skin::Kerbose;
            env.context.view.load_animation_file(sprite.Aframe, "Kerbose.ani");
        } else {
            skin = Skin::Putulo;
            env.context.view.load_animation_file(sprite.Aframe, "Putulo.ani");
            sprite.soundFile = "putulohurt";
            sprite.deathType = "putuloDeath";
        }
    }

    void animate(std::int64_t) override {
        if (dying) {
            return;
        }
        sprite.frame = sprite.frame + 1;
        if (sprite.frame > 2) { sprite.frame = 1; }
     }

    void death_animation() override {
        sprite.visible = true;

        sprite.kind = Kind::neutral;
        sprite.frame = 3;

        sprite.miscTime = env.current_time + 3;
        if (skin == Skin::Kerbose) {
            env.context.sound.PlaySound("Kerbose Die");
        } else if (skin == Skin::Putulo) {
            env.context.sound.PlaySound("putulodie");
        }
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

            sprite.visible = true;

            sprite.kind = Kind::neutral;
            sprite.frame = 3;

            sprite.miscTime = env.current_time + 3;
            if (skin == Skin::Kerbose) {
                env.context.sound.PlaySound("Kerbose Die");
            }
            else if (skin == Skin::Putulo) {
                env.context.sound.PlaySound("putulodie");
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

CharacterProc * create_kerbose_proc(
    CharacterProcEnv env,
    EntityManagerCO & e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new Kerbose(env, e_manager, position, texture_index);
}

}   }   }
