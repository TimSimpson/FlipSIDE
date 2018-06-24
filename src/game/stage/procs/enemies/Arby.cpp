#include "Arby.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class Arby : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    lp3::sims::CoroutineState state;
    bool dying;

public:
    Arby(CharacterProcEnv _env, EntityManagerCO e_manager,
            const glm::dvec3 & position,
            const int texture_index)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
        state(),
        dying(false)
    {
        sprite.name = "arby";
        sprite.set_position(position);
        sprite.texture = texture_index;
        sprite.proc = this;

        sprite.set_seek(position);
        sprite.wide = 80;
        sprite.high = 80;
        sprite.length = 40;
        sprite.frame = 1;
        sprite.mph = 1;
        sprite.hp = 5;
        sprite.speed = 0.25;
        sprite.visible = true;
        sprite.kind = Kind::goomba_thing;
        sprite.invTime = 1;
        sprite.soundFile = "Arby Ouch";
    }

    ~Arby()
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
        sprite.flickerTime = env.current_time + 10;
        sprite.seekx = sprite.wide * 3;
        sprite.mph = 2;
        sprite.kind = Kind::neutral;
        //sound.PlaySound(which);
        env.context.sound.PlaySound("dying explosion");
        dying = true;
    }

    bool update() override {
        LP3_COROUTINE_BEGIN(state)
            while (!dying) {
                seek(sprite);

                if (sprite.x == sprite.seekx && sprite.y == sprite.seeky) {
                    const int k = (int)(env.random.next() * 4) + 1;
                    sprite.seekx = sprite.x;
                    sprite.seeky = sprite.y;
                    if (k == 1) { sprite.seekx = sprite.x + 25; }
                    if (k == 2) { sprite.seekx = sprite.x - 25; }
                    if (k == 3) { sprite.seeky = sprite.y + 25; }
                    if (k == 4) { sprite.seeky = sprite.y - 25; }
                    if ((sprite.seekx + sprite.wide) > 640) { sprite.seekx = 640 - sprite.wide; }
                    if (sprite.seekx < 1) { sprite.seekx = 0; }
                    if ((sprite.seeky + sprite.high) > 480) { sprite.seeky = 480 - sprite.high; }
                    if (sprite.seeky < 1) { sprite.seeky = 0; }
                }
                LP3_YIELD(true);
            }
            while(sprite.wide >= sprite.seekx) {
                sprite.wide = sprite.wide + sprite.mph;
                sprite.x = sprite.x - (sprite.mph * 0.5 * fs_speed_factor);
                sprite.high = sprite.high + sprite.mph;
                sprite.y = sprite.y - (sprite.mph * 0.5 * fs_speed_factor);
                LP3_YIELD(true);
            }
            while(sprite.wide < sprite.seekx) {
                sprite.high = sprite.high - sprite.mph;
                sprite.y = sprite.y + (sprite.mph * 0.5 * fs_speed_factor);
                if (sprite.high < 1) {
                    sprite.visible = false;
                    break;
                }
                LP3_YIELD(true);
            }
        LP3_COROUTINE_END()
        return false;
    }
};

CharacterProc * create_arby_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new Arby(env, e_manager, position, texture_index);
}

}   }   }
