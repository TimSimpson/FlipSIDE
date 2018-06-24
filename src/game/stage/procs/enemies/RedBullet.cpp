#include "RedBullet.hpp"
#include <lp3/sims.hpp>

#include "BlueStick.hpp"

namespace nnd3d { namespace game { namespace proc {

namespace {
    static int dumber_index = 0;
}

class RedBullet : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    const int texture_index;
    lp3::sims::CoroutineState state;
    int dumb_index;
public:
    RedBullet(CharacterProcEnv _env,
               EntityManagerCO e_manager,
               const glm::dvec3 & position,
               const glm::dvec3 & seek_location,
               const int texture_index_arg)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
        texture_index(texture_index_arg),
        state(),
        dumb_index(dumber_index++)
    {
        sprite.proc = this;
        sprite.name = "bullet";
        sprite.set_position(position);
        sprite.texture = texture_index;

        env.context.view.load_animation_file(sprite.Aframe, "paulbullet.ani");

        sprite.mph = 2;
        sprite.frame = 1;
        sprite.kind = Kind::enemy_bullet;
        sprite.invTime = 1;
        sprite.hp = 999;
        sprite.wide = 10;
        sprite.high = 10;
        sprite.visible = true;

        sprite.flickOn = true;
        sprite.trueVisible = 0;
        sprite.zOrder = -90;

        sprite.set_seek(seek_location);
        LP3_LOG_INFO("[ %d ] welcome", dumb_index);
    }

    ~RedBullet() {
        sprite.proc = nullptr;
    }

    void animate(std::int64_t) override {
        sprite.visible = true;
        ++ sprite.frame;
        if (sprite.frame > 2) {
            sprite.frame = 1;
        }
     }

    void death_animation() override {

    }

    bool update() override {
        LP3_LOG_INFO("[ %d ] bullet pos %d, %d, size=%d, %d vis=%s, tex=%d, (%d, %d - %d, %d) trueVisible=%d, flickOn=%s, name=%s", dumb_index, sprite.x, sprite.y, sprite.wide, sprite.high, sprite.visible ? "true" : "false", sprite.texture, sprite.srcx, sprite.srcy, sprite.srcx2, sprite.srcy2, sprite.trueVisible, sprite.flickOn, sprite.name);
        sprite.visible = true;
        if (off_camera_kill(sprite, env.camera)) {
            LP3_LOG_INFO("[ %d ] bullet killed due to off-camera", dumb_index);
            return false;
        }

        // // TODO: figure out what this is accomplishing
        while(!(
            (sprite.seekx > env.camera.boundary().x || sprite.seekx < 0)
                || (sprite.seeky > env.camera.boundary().y || sprite.seeky < 0)
            )) {
            if (sprite.seekx > sprite.x) {
                sprite.seekx = sprite.seekx + ((sprite.seekx - sprite.x));
            }
            if (sprite.seekx < sprite.x) {
            sprite.seekx = sprite.seekx - ((sprite.x - sprite.seekx));
            }

            if (sprite.seeky > sprite.y) {
                sprite.seeky = sprite.seeky + ((sprite.seeky - sprite.y));
            }
            if (sprite.seeky < sprite.y) {
                sprite.seeky = sprite.seeky - ((sprite.y - sprite.seeky));
            }
        }

        seek(sprite);
        // sprite.frame = sprite.frame + 1; if (sprite.frame > 1) { sprite.frame = 0; }
        return true;
    }
};

CharacterProc * create_redbullet_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const glm::dvec3 & seek_location,
    const int texture_index)
{
    return new RedBullet(env, e_manager, position, seek_location, texture_index);
}

}   }   }
