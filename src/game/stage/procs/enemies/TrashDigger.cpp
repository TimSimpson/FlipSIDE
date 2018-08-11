#include "TrashDigger.hpp"
#include <lp3/sims.hpp>

#include "BlueStick.hpp"

namespace nnd3d { namespace game { namespace proc {


class TrashDigger : public CharacterProc {
private:
    CharacterProcEnv env;
    EntityManagerCO e_manager;
    CharacterSprite & sprite;
    const int texture_index;
    bool dying;
    lp3::sims::CoroutineState move_coro;
    lp3::sims::CoroutineState dying_coro;

public:
    TrashDigger(CharacterProcEnv _env,
                EntityManagerCO e_manager_arg,
                const glm::dvec3 & position,
                const int texture_index_arg)
    :   env(_env),
        e_manager(e_manager_arg),
        sprite(e_manager.grab_sprite()),
        texture_index(texture_index_arg),
        dying(false),
        move_coro(),
        dying_coro()
    {
        sprite.proc = this;
        sprite.name = "tdigger";
        sprite.set_position(position);
        sprite.texture = texture_index;


        env.context.view.load_animation_file(sprite.Aframe, "tDigger.ani");
        sprite.hp = 1;

        sprite.wide = 57;
        sprite.high = 37;
        sprite.visible = true;
        sprite.kind = Kind::enemy;
        sprite.frame = 1;

        sprite.mode = "";



        // sprite.invTime = 1;
        // sprite.soundFile = "Paul Ouch";
        // sprite.length = 20;


        // sprite.seekx = 0; // I guess?
        // sprite.proc = this;
    }

    void animate(std::int64_t) override {
        sprite.frame = sprite.frame + 1;
        if (sprite.mode == "") {
            if (sprite.frame > 5) { sprite.frame = 4; }
        }
        if (sprite.mode == "runner") {
            if (sprite.frame > 2) { sprite.frame = 1; }
        }
     }

    void death_animation() override {
        env.context.sound.PlayWave("WhaWhee.wav");
        sprite.reverse = false;
        dying = true;
    }

    void move() {
        LP3_COROUTINE_BEGIN(move_coro)
            while(true) {
                if (sprite.mode == "") {
                    unstretch(sprite);
                    //TSNOW: Another funky step 10 loop.
                    //2018-06: This code makes no sense but I think it's what it
                    // used to do. The dog stops when it horizontally aligns with
                    // the player... but WHYYYYYY.
                    auto players = e_manager.find_by_kind(Kind::player);
                    for (const auto & p : players) {
                        if ((p.get().y + p.get().high) >= sprite.y && p.get().y < sprite.y) {
                            sprite.mode = "runner";
                            sprite.seekx = 6000;
                            //.mhp = 10
                            sprite.kind = Kind::enemy;
                            sprite.deathType = "expand";
                            sprite.time = 1;
                            //.hp = 1
                            sprite.reverse = true;
                            break;
                        }
                    }
                }

                if (!sprite.target) {
                    sprite.target = e_manager.find_closest_player(this->sprite);
                    sprite.seekx = sprite.getMiddleX();
                    sprite.seeky = sprite.getMiddleY();
                }

                if (sprite.mode == "runner") {
                    if (sprite.target) {
                        if (getProx(sprite, *(sprite.target)) > 50) {
                            sprite.seekx = sprite.target->getMiddleX();
                        }
                        if (getProx(sprite, *(sprite.target)) < 50) {
                            sprite.mph = sprite.mph - 1;
                            if (sprite.mph < -90) {
                                sprite.seekx = sprite.target->getMiddleX();
                                sprite.mph = 10;
                            }
                        }
                        sprite.seeky = sprite.target->getMiddleY();
                        if (getProx(sprite, *(sprite.target)) < 100) {
                            sprite.mph = lp3::narrow<int>(getProx(sprite, *(sprite.target)) / 100);
                        }
                        else {
                            sprite.mph = 10;
                        }
                        if (getProx(sprite, *(sprite.target)) == 0) {
                            sprite.seekx = sprite.target->x;
                            sprite.seeky = sprite.target->y;
                        }

                        seek(sprite);
                    }
                }
                LP3_YIELD();
            }
        LP3_COROUTINE_END()
    }

    bool explode() {
        LP3_COROUTINE_BEGIN(dying_coro)
            while(sprite.time <= 2) {
                sprite.kind = Kind::neutral;
                //if (sprite.mode = "runner") then
                sprite.frame = 3;
                sprite.time += (0.01 * fs_speed_factor);
                sprite.wide = sprite.wide + (fs_speed_factor);
                sprite.x = sprite.x - (fs_speed_factor / 2);
                sprite.high = sprite.high + (fs_speed_factor);
                sprite.y = sprite.y - (fs_speed_factor / 2);
                LP3_YIELD(true);
            }

            sprite.name = "harharhar";
            sprite.flickerTime = env.current_time + 2;
            sprite.time = 2;  // 2018-06: using this instead of world.clock
            env.context.sound.PlayWave("harharhar.wav");
            dying = true;

            while(sprite.time > 0) {
                sprite.time -= 0.016f;  // 2018-06: using this instead of world.clock
            }
            kill(sprite);
        LP3_COROUTINE_END()
        return false;
    }

    bool update() override {
        if (!dying) {
            move();
            return true;
        } else {
            return explode();
        }
    }
};

CharacterProc * create_trashdigger_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index)
{
    return new TrashDigger(env, e_manager, position, texture_index);
}

}   }   }
