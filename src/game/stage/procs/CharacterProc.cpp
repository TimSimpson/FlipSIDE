#include "CharacterProc.hpp"

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


////void create_player(
////    PlayerData & player_data, CharacterSprite & sprite,
////    gsl::span<std::reference_wrapper<CharacterSprite>> & children,
////    CharacterProcEnv & env)
////{
////    LP3_ASSERT(children.size() == 9);
////
////    if (player_data.playerName == "Thomas") {
////        player_data.weapon = "fireball";
////        env.context.view.load_animation_file(sprite.Aframe, "Thomas.ani");
////        env.context.view.LoadTexture(player_data.sprite_index + 1, "Flip1.png", 254, 254);
////        sprite.texture = player_data.sprite_index + 1;
////        for (CharacterSprite & child : children) {
////            env.view.load_animation_file(child.Aframe, "Fireball.ani");
////        }
////    }
////
////    if (player_data.playerName == "Nick") {
////        player_data.weapon = "fireball";
////        env.view.load_animation_file(sprite.Aframe, "nick.ani");
////        env.view.LoadTexture(player_data.sprite_index + 1, "joel.png", 254, 258);
////        sprite.texture = player_data.sprite_index + 1;
////        for (CharacterSprite & child : children) {
////            env.view.load_animation_file(child.Aframe, "icespike.ani");
////        }
////    }
////
////    if (player_data.playerName == "Nicky") {
////        player_data.weapon = "bomb";
////        env.view.load_animation_file(sprite.Aframe, "nicky.ani");
////        env.view.LoadTexture(player_data.sprite_index + 1, "LilNicky.png", 84, 148);
////        sprite.texture = player_data.sprite_index + 1;
////        for (CharacterSprite & child : children) {
////            env.view.load_animation_file(child.Aframe, "bomb.ani");
////        }
////    }
////
////    for (CharacterSprite & child : children) {
////        child.name = "";
////        child.zOrder = -90;
////    }
////}

// For starters, all the old code is just shoved in here.
class LegacyProc : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & s;
    int j;
    bool mover;
    World & world;

public:
     LegacyProc(CharacterProcEnv & _env, CharacterSprite & _s, int _j,
                World & world_arg, const std::string & name)
     :  env(_env),
        s(_s),
        j(_j),
        mover(false),
        world(world_arg)
     {
        s.name = name;
        s.proc = this;
        initialize();
     }

     ~LegacyProc() override = default;

     void animate(std::int64_t) override {
         //TODO: animation_timer was set to fire every 200ms- basically do
         //      something similar, move this to view, and call it outside of
         //      the logic loop.

         if (s.name == "goomba" || s.name == "Kerbose"
             || s.name == "paulrun" || s.name == "pigeonbomber") {
             s.frame = s.frame + 1;
             if (s.frame > 2) { s.frame = 1; }
         }

         if (s.name == "pigeon") {
             s.frame = s.frame + 1;
             if (s.frame > 2) { s.frame = 1; }
         }

         if (s.name == "tdigger") {
             s.frame = s.frame + 1;
             if (s.mode == "") {
                 if (s.frame > 5) { s.frame = 4; }
             }
             if (s.mode == "runner") {
                 if (s.frame > 2) { s.frame = 1; }
             }
         }

         if (s.name == "bluestick") {
             s.frame = s.frame + 1;
             if (s.frame > 2) { s.frame = 1; }
         }
    }


    void death_animation() override {
        s.name = s.deathType;
        initialize();
    }

    void initialize() override {
        view::View & view = env.context.view;
        Sound & sound = env.context.sound;
        const double current_time = env.current_time;

        auto & spr = this->s;

        //this makes all the sprites do their thing based on their name


        if (spr.name == "tdigger") {
            view.load_animation_file(spr.Aframe, "tDigger.ani");
            spr.hp = 1;
        }

        if (spr.name == "clouds") {
            spr.Aframe[1].set(spr.srcx, spr.srcy, spr.srcx2, spr.srcy2);
        }


        if (spr.name == "paulrun") {
            view.load_animation_file(spr.Aframe, "paulrun.ani");
            spr.hp = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.deathType = "gotmilkbs";
            spr.invTime = 1;
            spr.hp = 2;
            spr.soundFile = "Paul Ouch";
            spr.length = 20;
        }


        if (spr.name == "paulbullet") {
            view.load_animation_file(spr.Aframe, "paulbullet.ani");
            spr.hp = 999;
            spr.kind = Kind::enemy;
            spr.deathType = "Kerbose";
            spr.invTime = 0;
            spr.wide = 10;
            spr.high = 10;
            spr.name = "bullet";
            spr.mph = 2;
        }


        if (spr.name == "bs death") {
            spr.visible = true;
            this->mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            spr.miscTime = current_time + 3;
            sound.PlaySound("stick die");
            spr.name = "Kerbose Death";
        }


        if (spr.name == "harharhar") {
            spr.flickerTime = current_time + 2;
            spr.time = 2;  // 2018-06: using this instead of world.clock
            sound.PlayWave("harharhar.wav");
        }

        if (spr.name == "expand") {
            sound.PlayWave("WhaWhee.wav");
            spr.reverse = false;
        }

        if (spr.name == "cinema") {
            spr.visible = false;
            spr.frame = 8;
            unstretch(spr);
            spr.mode = "";
        }

        if (spr.name == "Dying Explosion") {
            //StopSound which
            //LoadSound which, "goombaspr.wav"
            spr.flickerTime = current_time + 10;
            spr.seekx = spr.wide * 3;
            spr.mph = 2;
            spr.kind = Kind::neutral;
            //sound.PlaySound(which);
            sound.PlaySound("dying explosion");
        }

        if (spr.name == "gotmilkbs") {
            sound.PlaySound("Paul Shrink");
            spr.name = "bluestick";
        }

        if (spr.name == "greenspring") {
            view.load_animation_file(spr.Aframe, "greenspring.ani");
            spr.frame = 1;
            spr.mode = "off";
            spr.hp = 1;
            spr.deathType = "greenspring";
			spr.time = 0;
            spr.kind = Kind::trampoline;
            spr.jumpM = 1.5;
            spr.length = 10;
            spr.wide = spr.wide * 2;
            spr.high = spr.high * 2;
        }


        if (spr.name == "bluestick") {
            spr.hp = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.wide = 10;
            spr.high = 17;
            spr.length = 10;
            spr.Aframe[1].set(1, 173, 10, 190);
            spr.Aframe[2].set(13, 173, 23, 190);
            spr.Aframe[3].set(23, 174, 33, 190);
            spr.deathType = "bs death";
            spr.soundFile = "Stick Ouch";
            //spr.name = "Kerbose"
            sound.PlaySound("Stick Awaken");
            spr.frame = 1;
            unstretch(spr);
        }

        if (spr.name == "fireball") {
            spr.wide = 40;
            spr.high = 50;
            spr.visible = false;
            spr.texture = 1;
            spr.frame = 1;
            spr.name = "";
            spr.zOrder = -99;
        }



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

        if (spr.name == "falling") {
            spr.kind = Kind::neutral;
            //spr.high = -30 //-1 * spr.high
        }

        if (spr.name == "pigeon") {
            spr.visible = true;
            spr.seekx = spr.x;
            spr.seeky = spr.y;
            spr.wide = 40;
            spr.high = 30;
            spr.hp = 1;
            spr.z = 80;
            spr.length = 30;
            spr.frame = 1;
            spr.deathType = "falling";
            spr.kind = Kind::enemy_weak_to_jumping;
        }

        if (spr.name == "pigeonbomber") {
            view.load_animation_file(spr.Aframe, "pigeon.ani");
            spr.visible = true;
            spr.seekx = -10;
            spr.seeky = spr.y;
            spr.wide = 40;
            spr.high = 30;
            spr.hp = 1;
            spr.z = 80;
            spr.length = 30;
            spr.frame = 1;
            spr.deathType = "falling";
            spr.kind = Kind::enemy_weak_to_jumping;
        }

        if (spr.name == "goomba") {
            spr.seekx = spr.x;
            spr.seeky = spr.y;
            spr.wide = 80;
            spr.high = 80;
            spr.length = 40;
            spr.texture = 6;
            spr.frame = 1;
            spr.mph = 1;
            spr.hp = 5;
            spr.speed = 0.25;
            spr.visible = true;
            spr.kind = Kind::goomba_thing;
            spr.invTime = 1;
            spr.deathType = "Dying Explosion";
            spr.soundFile = "Goomba Ouch";
            //LoadSound which, spr.soundFile
            this->mover = true;
        }

        if (spr.name == "bullet") {
            spr.kind = Kind::enemy_bullet;
        }
    }

    void shoot(CharacterSprite & who, // This was an integer
		       const std::string & what, int wherex, int wherey) {

        // 2018-06 : the old code would search through the sprites list, find
        //           one that looked dead, and re-initialize it as a "child"
        //           of some parent Sprite.
        //           I want to resurrect this, but instead have the parent's
        //           process "own" all the potential children sprites it
        //           might have to make the way things are laid out
        //           more deterministic.
        //           For now, that ain't happening!

        // int opera;

        // for (opera = (who + 1); opera < lp3::narrow<int>(world.Sprite.size()); ++ opera) {
        //     if (world.Sprite[opera].name == "" || world.Sprite[opera].name == "empty" || world.Sprite[opera].name == "dead") {
        //         // killS opera
        //         world.Sprite[opera].name = what;
        //         break;
        //     }
        // }

        // if (opera >= 95) { return; }  //2017: WAT?

        // world.Sprite[opera].trueVisible = 0;
        // world.Sprite[opera].visible = true;
        // world.Sprite[opera].flickOn = false;
        // world.Sprite[opera].texture = world.Sprite[who].texture;
        // world.Sprite[opera].wide = world.Sprite[who].wide;
        // world.Sprite[opera].high = world.Sprite[who].high;

        // world.Sprite[who].proc->spawn(world.Sprite[opera], what);
        // world.Sprite[opera].zOrder = -1;
        // world.Sprite[opera].x = world.Sprite[who].x;
        // world.Sprite[opera].y = world.Sprite[who].y;
        // world.Sprite[opera].z = world.Sprite[who].z;
        // world.Sprite[opera].seekx = wherex;
        // world.Sprite[opera].seeky = wherey;
    }

    bool update() override
        //Camera & camera,
        //PlayerData * player_data,
        //gsl::span<std::reference_wrapper<CharacterSprite>> & children) override
    //  gsl::span<std::reference_wrapper<CharacterSprite>> & children) override
    {
        //auto & view = env.context.view;
        //auto & sound = env.context.sound;
        auto & random = env.random;
        int k;

        // this came from levelR-
        if (s.name == "greenspring") {
            if (s.mode == "bounce") {
                s.frame = s.frame + 1;
                if (s.frame > 5) { s.frame = 2; }
				s.time += fs_s_per_update;
                if (s.miscTime < s.time) { s.mode = ""; s.frame = 1; }
            }
        }

        if (s.name == "clouds") {
            s.srcx = s.srcx + (fs_speed_factor * 0.5);
            s.srcx2 = s.srcx2 + (fs_speed_factor * 0.5);
            s.Aframe[1].x = s.Aframe[1].x + 1;
            s.Aframe[1].x2 = s.Aframe[1].x2 + 1;
        }

        if (s.name == "bullet") {     ////This is a strange type of bullet that in retrospect feels more like a bubble
            //if (ws.seekx <> -1) then

            off_camera_kill(s, world.camera);

            while(!(
                (s.seekx > world.camera.boundary().x || s.seekx < 0)
                    || (s.seeky > world.camera.boundary().y || s.seeky < 0)
                )) {
                if (s.seekx > s.x) {
                    s.seekx = s.seekx + ((s.seekx - s.x));
                }
                if (s.seekx < s.x) {
                s.seekx = s.seekx - ((s.x - s.seekx));
                }

                if (s.seeky > s.y) {
                    s.seeky = s.seeky + ((s.seeky - s.y));
                }
                if (s.seeky < s.y) {
                    s.seeky = s.seeky - ((s.y - s.seeky));
                }
            }

			seek(s);
            s.frame = s.frame + 1; if (s.frame > 1) { s.frame = 0; }
        }

        if (s.name == "paulrun") {
            if (s.mode == "") { s.mode = "right"; }

            if (s.mode == "right") {
                s.reverse = false;
                s.x = s.x + fs_speed_factor;
                s.seekx = s.seekx + fs_speed_factor;
                if (s.seekx > 100) {
                    s.mode = "left"; s.seekx = 0; s.dir = ""; }
            }
            if (s.mode == "left") {
                s.reverse = true;
                s.x = s.x - fs_speed_factor;
                s.seekx = s.seekx + fs_speed_factor;
                if (s.seekx > 100) {
                    s.mode = "right"; s.seekx = 0; s.dir = ""; }
            }

            if (s.seekx >= 50 && s.dir != "done") {
                const auto target = world.find_closest_player(s);
                if (target) {
                    this->shoot(s, "paulbullet",
                                target->x,
                                target->y);
                }
                s.dir = "done";
            }
        }

        if (s.name == "bluestick") {
            k = random.next() * 2 + 1;
            if (k == 1) { s.x = s.x + fs_speed_factor; }
            if (k == 2) { s.x = s.x - fs_speed_factor; }
            k = random.next() * 2 + 1;
            if (k == 1) { s.y = s.y + fs_speed_factor; }
            if (k == 2) { s.y = s.y - fs_speed_factor; }
            k = random.next() * 20 + 1;
            if (k == 1) { if (s.z == 0) {
                start_jump(s); } }
        }

        if (s.name == "pigeonbomber") {
            s.z = s.z + fs_speed_factor;
            //s.frame = s.frame + 1
            //if (s.frame > 2) then s.frame = 1

            seek(s);
            if (s.x < 1) { s.x = world.camera.boundary().x; }

            if (s.miscTime < this->env.current_time) {
                const auto target
                    = world.find_closest_player(s);
                if (target) {
                    this->shoot(s, "bluestick",
                                target->x,
                                target->y);
                }
                s.miscTime = this->env.current_time + 2;
            }
        }

        // end levelR stuff
        if (s.name == "goomba") {
            seek(s);

            if (s.x == s.seekx && s.y == s.seeky) {
                k = (int)(random.next() * 4) + 1;
                s.seekx = s.x;
                s.seeky = s.y;
                if (k == 1) { s.seekx = s.x + 25; }
                if (k == 2) { s.seekx = s.x - 25; }
                if (k == 3) { s.seeky = s.y + 25; }
                if (k == 4) { s.seeky = s.y - 25; }
                if ((s.seekx + s.wide) > 640) { s.seekx = 640 - s.wide; }
                if (s.seekx < 1) { s.seekx = 0; }
                if ((s.seeky + s.high) > 480) { s.seeky = 480 - s.high; }
                if (s.seeky < 1) { s.seeky = 0; }
            }
        }

        if (s.name == "falling") {
            //.flickerTime = clock + 1
            s.z = s.z - fs_speed_factor;
            if (s.z < 1) {
                s.z = 1;
                s.name = "deceased";
                s.visible = false;
                s.trueVisible = 2;
                s.flickerTime = 0;
            }
            s.frame = s.frame + 1;
            if (s.frame > 4) { s.frame = 3; }
        }

        if (s.name == "Dying Explosion") {
            if (s.wide < s.seekx) {
                s.wide = s.wide + s.mph;
                s.x = s.x - (s.mph * 0.5 * fs_speed_factor);
                s.high = s.high + s.mph;
                s.y = s.y - (s.mph * 0.5 * fs_speed_factor);
            }
            if (s.wide >= s.seekx) {
                s.high = s.high - s.mph;
                s.y = s.y + (s.mph * 0.5 * fs_speed_factor);
                if (s.high < 1) {
                    s.name = "deceased";
                    s.visible = false;
                }
            }
        }

        if (s.name == "tdigger") {
            if (s.mode == "") {
                unstretch(s);
                //TSNOW: Another funky step 10 loop.
                //2018-06: This code makes no sense but I think it's what it
                // used to do. The dog stops when it horizontally aligns with
                // the player... but WHYYYYYY.
                auto players = world.find_by_kind(Kind::player);
                for (const auto & p : players) {
                    if ((p.get().y + p.get().high) >= s.y && p.get().y < s.y) {
                        s.mode = "runner";
                        s.seekx = world.camera.boundary().x;
                        //.mhp = 10
                        s.kind = Kind::enemy;
                        s.deathType = "expand";
                        s.time = 1;
                        //.hp = 1
                        s.reverse = true;
                        break;
                    }
                }
            }

            if (!s.target) {
                s.target = world.find_closest_player(this->s);
                s.seekx = s.getMiddleX();
                s.seeky = s.getMiddleY();
            }

            if (s.mode == "runner") {
                if (s.target) {
                    if (getProx(s, *(s.target)) > 50) {
                        s.seekx = s.target->getMiddleX();
                    }
                    if (getProx(s, *(s.target)) < 50) {
                        s.mph = s.mph - 1;
                        if (s.mph < -90) {
                            s.seekx = s.target->getMiddleX();
                            s.mph = 10;
                        }
                    }
                    s.seeky = s.target->getMiddleY();
                    if (getProx(s, *(s.target)) < 100) {
                        s.mph = (getProx(s, *(s.target)) / 100);
                    }
                    else {
                        s.mph = 10;
                    }
                    if (getProx(s, *(s.target)) == 0) {
                        s.seekx = s.target->x;
                        s.seeky = s.target->y;
                    }

                    seek(s);
                }
            }

        }


        if (s.name == "expand") {
            s.kind = Kind::neutral;
            //if (s.mode = "runner") then
            s.frame = 3;
            s.time += (0.01 * fs_speed_factor);
            s.wide = s.wide + (fs_speed_factor);
            s.x = s.x - (fs_speed_factor / 2);
            s.high = s.high + (fs_speed_factor);
            s.y = s.y - (fs_speed_factor / 2);
            if (s.time > 2) {
                s.name = "harharhar";
                initialize();
                // load_process(env, s, s.name); //: killS j
            }
        }

        if (s.name == "harharhar") {
            s.time -= 0.016f;  // 2018-06: using this instead of world.clock
            if (s.time <= 0) {
                kill(s);
            }
        }

        if (s.name == "dead") {
            //Stop
            if ((j / 10) == 0) {
                s.x = world.camera.x() + 10;
            }
            if ((j / 10) == 1) {
                s.x = world.camera.x() + 250;
                s.color = view::qb_color(10);
            }
            if ((j / 10) == 2) {
                s.x = world.camera.x() + 450;
                s.color = view::qb_color(14);
            }
            s.y = world.camera.y() + 10;
            s.frame = 0;
            s.visible = false;
            s.srcx = 2;
            s.srcy = 363;
            s.srcx2 = 96;
            s.srcy2 = 379;
            s.texture = 0;
            s.visible = true;
        }
        return true;
    }

    // Create a child process (think bullets)
    CharacterProc * spawn(CharacterSprite & sprite,
                          const std::string & name) override {
        // For now, just return the same class as it doesn't matter.
        //child = this;
        sprite.proc = this; // child;
        sprite.name = name;
        sprite.proc->initialize();
        return this;
    }
};

//void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &) {
//  // TODO: Pass the name in rather than relying on the name variable.
//  s.proc = load_process(s.name);
//  s.proc->initialize(env, s);
//}


CharacterProcManager::CharacterProcManager()
:   procs()
{
}

CharacterProcManager::~CharacterProcManager() {
    clear();
}

CharacterProcManager::CharacterProcManager(CharacterProcManager && rhs)
:   procs(std::move(rhs.procs)) {
}



void CharacterProcManager::add_process(gsl::owner<CharacterProc *> process) {
    // TODO: in the future, use the names here, or something, but for
    //       now, use the indexes to figure out if it's a player
    if (process != nullptr) {
        procs.push_back(process);
    }
}

void CharacterProcManager::animate(std::int64_t ms) {
    for (auto & p : procs) {
        p->animate(ms);
    }
}

void CharacterProcManager::clear() {
    for (gsl::owner<CharacterProc *> p : procs) {
        delete p;
    }
    procs.clear();
}

void CharacterProcManager::update() {
    for (int index = 0; lp3::narrow<std::size_t>(index) < procs.size();
            ++ index) {
        if (!procs[index]->update()) {
            delete procs[index];
            procs.erase(procs.begin() + index);
            index -= 1;
        }
    }
}


gsl::owner<CharacterProc *> legacy_add_process(
    CharacterProcEnv & env, World & world, EntityManager &,
    int j, CharacterSprite & s, const std::string & name)
{
    // TODO: in the future, use the names here, or something, but for
    //       now, use the indexes to figure out if it's a player
    return new LegacyProc(env, s, j, world, name);
}

}   }
