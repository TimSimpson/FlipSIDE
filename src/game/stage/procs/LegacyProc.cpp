#include "LegacyProc.hpp"

#include "PlayerProc.hpp"

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {
    const glm::vec4 normColor{ 1.0f, 1.0f, 1.0f, 1.0f };
}


// For starters, all the old code is just shoved in here.
class LegacyProc : public CharacterProc {
private:
    CharacterProcEnv env;
    EntityManagerCO entity_manager;
    CharacterSprite & s;
    bool mover;
    World & world;

public:
     LegacyProc(CharacterProcEnv & _env, EntityManagerCO & entity_manager_arg,
                CharacterSprite & _s,
                World & world_arg, const std::string & name)
     :  env(_env),
        entity_manager(entity_manager_arg),
        s(_s),
        mover(false),
        world(world_arg)
     {
        s.name = name;
        s.proc = this;
        initialize();
     }

     ~LegacyProc() override = default;

    void animate(std::int64_t) override {

    }


    void death_animation() override {
        s.name = s.deathType;
        initialize();
    }

    void initialize() {
        auto & spr = this->s;

        //this makes all the sprites do their thing based on their name

        if (spr.name == "clouds") {
            spr.Aframe[1].set(spr.srcx, spr.srcy, spr.srcx2, spr.srcy2);
        }

   //      if (spr.name == "greenspring") {
   //          view.load_animation_file(spr.Aframe, "greenspring.ani");
   //          spr.frame = 1;
   //          spr.mode = "off";
   //          spr.hp = 1;
   //          spr.deathType = "greenspring";
			// spr.time = 0;
   //          spr.kind = Kind::trampoline;
   //          spr.bounce_factor = 1.5;
   //          spr.length = 10;
   //          spr.wide = spr.wide * 2;
   //          spr.high = spr.high * 2;
   //      }

        if (spr.name == "Deadly Rat") {
            spr.wide = 50;
            spr.high = 50;
            spr.texture = 0;
            spr.name = "Deadly Rat";
            spr.hp = 5;
            spr.visible = true;
            spr.kind = Kind::enemy;
            spr.deathType = "Dying Explosion";
            spr.invTime = 1;
        }

    }

    bool update() override {

        // this came from levelR-
    //     if (s.name == "greenspring") {
    //         if (s.mode == "bounce") {
    //             s.frame = s.frame + 1;
    //             if (s.frame > 5) { s.frame = 2; }
				// s.time += fs_s_per_update;
    //             if (s.miscTime < s.time) { s.mode = ""; s.frame = 1; }
    //         }
    //     }

        if (s.name == "clouds") {
            s.srcx = s.srcx + (fs_speed_factor * 0.5);
            s.srcx2 = s.srcx2 + (fs_speed_factor * 0.5);
            s.Aframe[1].x = s.Aframe[1].x + 1;
            s.Aframe[1].x2 = s.Aframe[1].x2 + 1;
        }

        return true;
    }

};


gsl::owner<CharacterProc *> legacy_add_process(
    CharacterProcEnv & env, World & world, EntityManagerCO & entity_manager,
    const std::string & name,
    const SpriteLevelData & lvl_data)
{
    CharacterSprite & s = entity_manager.grab_sprite();
    s.name = lvl_data.name;
    s.x = lvl_data.position.x;
    s.y = lvl_data.position.y;
    s.z = lvl_data.position.z;
    s.srcx = lvl_data.src_ul.x;
    s.srcy = lvl_data.src_ul.y;
    s.srcx2 = lvl_data.src_br.x;
    s.srcy2 = lvl_data.src_br.y;
    s.wide = lvl_data.size.x;
    s.high = lvl_data.size.y;
    s.length = lvl_data.size.z;
    s.texture = lvl_data.texture.value;
    s.visible = lvl_data.visible;
    s.kind = lvl_data.kind;
    s.zOrder = lvl_data.z_order;

    // TODO: in the future, use the names here, or something, but for
    //       now, use the indexes to figure out if it's a player
    return new LegacyProc(env, entity_manager, s, world, name);
}

}   }
