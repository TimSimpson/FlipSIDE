#include "LegacyGame.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "BaseScreen.hpp"
#include "procs/CharacterProc.hpp"
#include "procs/CinemaProc.hpp"
#include "procs/Enemies.hpp"
#include "procs/PlayerProc.hpp"
#include "EntityManager.hpp"
#include "GameOverScreen.hpp"
#include "NowLoading.hpp"
#include "TitleScreen.hpp"

#ifdef _MSC_VER
    // Avoid the zillions implicit conversion warnings
    #pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {
    const glm::vec4 normColor{1.0f, 1.0f, 1.0f, 1.0f};
}


class LegacyGame : public GameProcess
{
private:
    GameContext context;
    Vb vb;
    view::View & view;
    Sound & sound;
    World world;
    Random random;
    std::int64_t animation_timer;
    EntityManager entity_manager;
    CharacterProcManager proc_manager;
    // This handles exitting to the next screen.
    bool exitS;
public:
    LegacyGame(GameContext _context,  World && world_arg)
    :   context(_context),
        vb(context.media),
        view(context.view),
        sound(context.sound),
        world(world_arg),
        random(),
        animation_timer(0),
        entity_manager(world),
        proc_manager(),
        exitS(false)
    {
        LP3_ASSERT(boost::starts_with(world.screen, "Level"));

        sound.silence_sfx();

        std::string crapple = world.screen.substr(5);
        double boogeycrap = boost::lexical_cast<double>(crapple);

        this->goToLevel(boogeycrap);
    }

    void handle_input(const input::Event & event) override {
        switch(event.key) {
            case input::Key::up:
                world.player_data[event.player].upKey = event.value != 0.0f;
                break;
            case input::Key::down:
                world.player_data[event.player].DownKEY = event.value != 0.0f;
                break;
            case input::Key::left:
                world.player_data[event.player].LeftKEY = event.value != 0.0f;
                break;
            case input::Key::right:
                world.player_data[event.player].RightKEY = event.value != 0.0f;
                break;
            case input::Key::attack:
                world.player_data[event.player].AttackKey = event.value != 0.0f;
                break;
            case input::Key::jump:
                world.player_data[event.player].JumpKey = event.value != 0.0f;
                break;
            case input::Key::skip_scene:
                if (event.value) {
                    this->exitS = true; sound.PlayWave("FDis.wav");
                }
                break;
            case input::Key::power_up:
                // Forget this for now as I remove all references to specific
                // sprite entries.

                // world.player_data[0].slicer = true;
                // world.player_data[0].GradeUp = 2;
                // world.Sprite[0].wide = 25;
                // world.Sprite[0].high = 25;
                // sound.PlayWave("SoupedUp.wav");
                break;
            case input::Key::lemon_time:
                world.LemonTime = true;
                break;
            default:
                LP3_ASSERT(false);
        }
    }

    gsl::owner<GameProcess *> update() override {
        set_time_stuff(world);

        int focus_x = 0;
        int focus_y = 0;

        this->findPlayers();

        if (world.numberPlayers.any_player_active()) {
            int itr_start;
            for (int i = 0; i < max_players; ++i) {
                if (world.numberPlayers.player[i]) {
                    itr_start = i;
                    break;
                }
            }
            LP3_ASSERT(itr_start < max_players);

            double max_x, min_x;
            max_x = min_x = world.Sprite[itr_start * 10].x
                + (world.Sprite[itr_start * 10].wide / 2);
            double max_y, min_y;
            max_y = min_y = world.Sprite[itr_start * 10].y
                + (world.Sprite[itr_start * 10].high / 2);

            for (int i = itr_start + 1; i < max_players; ++i) {
                if (world.numberPlayers.player[i]) {
                    min_x = std::min(min_x, world.Sprite[i * 10].x + (world.Sprite[i * 10].wide / 2));
                    max_x = std::max(max_x, world.Sprite[i * 10].x + (world.Sprite[i * 10].wide / 2));
                    min_y = std::min(min_y, world.Sprite[i * 10].y + (world.Sprite[i * 10].high / 2));
                    max_y = std::max(max_y, world.Sprite[i * 10].y + (world.Sprite[i * 10].high / 2));
                }
            }

            focus_x = (min_x + max_x) / 2;
            focus_y = (min_y + max_y) / 2;

            world.camera.focus({focus_x, focus_y});
        }


        //-----------------------------------------------------------
        //START OF NORMAL ROUTINE
        //------------------------------------------------------------

        // 2017: todo: try making these only appear in the loops that need them
        int j;
        int k;

        //Rem-FLICKER-
        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++j) {

            if (boost::starts_with(world.screen, "Level")) {
                int sopoer;
                //TSNOW: This may be wrong- old code was:
                //       sopoer = Val(right(left(screen, 6), 1))
                sopoer = boost::lexical_cast<double>(world.screen.substr(5, 1));
                this->levelR(sopoer, j);
            }
        }

        flicker(world);


        //Rem-------------------------------------------------------------------
        //               THIS SECTION DOES THE JUMPING
        //Rem-------------------------------------------------------------------

        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++j) {
            auto & s = world.Sprite[j];

            if (s.jumpTime != 0) {
                s.lastJump = s.z;
                //z=jt+(JS*T*2)-(g*t)*2^2
                if (s.jumpM == 0) { s.jumpM = 1; }
                s.z = s.jumpStart
                    + (
                    (s.jumpStrength * s.jumpM)
                        * ((world.clock - s.jumpTime) * 2)
                        )
                    - (
                        world.Gravity
                        * std::pow(((world.clock - s.jumpTime) * 2), 2)
                        );
                //
                if (s.z < 0) { s.z = 0; s.jumpTime = 0; s.jumpM = 1; }
            }
            else {

                //REM------------------------------------------------------
                //  THis is the added gravity that pulls them down   if the're in the ares.
                if (s.z > 0) { s.z = s.z - fs_speed_factor; }
            }
        }


        // CharacterProcEnv env{ context, random, world.clock, camera };
        proc_manager.update();


        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++j) {
            auto & s = world.Sprite[j];
            // Handle all "level" stuff here, but call update on gameproc
            // otherwise.
            // TODO: maybe what's needed here is a different abstraction, which
            //       could be a manager proc for each room that knows about
            //       all the entities and can script events.
            if (s.name == "frontdoor") {
                if (this->findQ("Kerbose") < 1) {
                    kill(s);
                    s.name = "exit";
                    CharacterProcEnv env{ context, random, world.clock, world.camera };

                    // This will break if anything else happens, but is needed
                    // to preserve the old way things were.
                    entity_manager.go_back(30);
                    proc_manager.add_process(
                        proc::create_cinema_proc(env, entity_manager, 1.1f));
                }
            } else if (s.name == "exit") {
                // This logic is rotten and only works between now and when
                // later code realizes the player is too far inside of the door
                // and pushes them back.
                for (int penguin = 0; penguin <= 2; penguin += 1) {
                    if (hit_detection(s, world.Sprite[penguin * 10]) != 0
                        && world.Sprite[penguin].name
                        == world.player_data[penguin].playerName) {
                        this->exitS = true;
                    }
                }
            } else if (s.proc) {
                //CharacterProcEnv env{context, random, world.clock};
                //s.proc->update(env, s, j, world);
            }

        }

        //TSNOW: end of the emulated with statement that creates variable "s",
        //       along with the for loop that used the "j" variable.
        //       Holy crap, after formatting that it's 1259 lines long.
        //       In Visual Basic it was 910 lines long.

        //---------------------------------------------------------------------
        //      END OF AN ERA
        //---------------------------------------------------------------------
        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++ j) {
            //If Sprite(j).mover = True Then
            for (k = j + 1; k < lp3::narrow<int>(world.Sprite.size()); ++k) {
                if (world.Sprite[j].kind == Kind::neutral) { goto fthis2; }
                if (world.Sprite[k].kind == world.Sprite[j].kind
                    || world.Sprite[k].kind == Kind::neutral) {
                    goto fthis2;
                }
                //If Sprite(k).mover = False And Sprite(j).mover = False Then GoTo fthis2
                //If k = 1 And j = 31 Then Stop
                if (this->hitdetection(j, k) != 0) {
                    this->checkHit(j, k);
                }
            fthis2:
                EMPTY_LABEL_HACK
            }

        }

        gsl::owner<GameProcess *> result = this->flipGame();
        if (result) {
            return result;
        }

        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++ j) {
            world.Sprite[j].lastX = world.Sprite[j].x;
            world.Sprite[j].lastY = world.Sprite[j].y;
        }
        animate();
        create_billboards(world, view.resolution, view.billboards());

        return nullptr;
    }

private:
    void animate() {
        animation_timer += 16;
        // emulates old timer that fired once every 200 ms
        if (animation_timer < 200) {
            return;
        }
        animation_timer -= 200;

        for (std::size_t j = 0; j < world.Sprite.size(); ++j) {
            auto & s = world.Sprite[j];
            if (s.proc) {
                s.proc->animate(200);
            }
        }
    }

    void checkHit(int j, int k) {
                bool redo = false;
        int HolderJ = 0;

        auto gosub_hurtj = [this, &HolderJ, &j]() {
            HolderJ = j;
            world.Sprite[HolderJ].flickerTime
                = world.clock + world.Sprite[HolderJ].invTime;
            sound.PlaySound(world.Sprite[HolderJ].soundFile);
        };

        auto gosub_hurtK = [this, &HolderJ, &k]() {
            HolderJ = k;
            world.Sprite[HolderJ].flickerTime
                = world.clock + world.Sprite[HolderJ].invTime;
            sound.PlaySound(world.Sprite[HolderJ].soundFile);
        };

    redo2:
        //If k = 1 And j = 31 Then Stop

        //Player hits an enemy
        if (world.Sprite[j].kind == Kind::player && (world.Sprite[k].kind == Kind::enemy
            || world.Sprite[k].kind == Kind::enemy_bullet)) {
            if (world.Sprite[j].flickerTime < world.clock) {
                world.Sprite[j].hp = world.Sprite[j].hp - 1;
                sound.PlaySound(world.Sprite[j].soundFile);
                gosub_hurtj();
                if (world.Sprite[j].hp <= 0) {
                    world.Sprite[j].proc->death_animation();
                }
            }
        }

        //Enemy hit by a fireball
        if ((world.Sprite[j].kind == Kind::enemy || world.Sprite[j].kind == Kind::enemy_weak_to_jumping)
            && world.Sprite[k].kind == Kind::fireball) {

            if (world.Sprite[j].flickerTime < world.clock) {
                world.Sprite[j].hp = world.Sprite[j].hp - 1;
                gosub_hurtj();
                if (world.Sprite[j].hp <= 0) {
                    world.Sprite[j].proc->death_animation();
                }
            }
            if (world.Sprite[k].name == "fireball") {
                if (world.player_data[(world.Sprite[k].parent) / 10].slicer == false) {
                    kill(world.Sprite[k]);
                }
            }
        }

        //Player steps on a goomba type thing
        if (world.Sprite[j].kind == Kind::player
            && (world.Sprite[k].kind == Kind::goomba_thing || world.Sprite[k].kind == Kind::enemy_weak_to_jumping)) {
            if (world.Sprite[j].z > world.Sprite[k].length
                && world.Sprite[j].lastJump > world.Sprite[j].z) {
                world.Sprite[j].jumpStart = world.Sprite[j].z;  //sends thing up into the air
                world.Sprite[j].jumpTime = world.clock; //sends thing up into the air, even if (goomba is flashing
                if (world.Sprite[k].flickerTime < world.clock) {
                    world.Sprite[k].hp = world.Sprite[k].hp - 1;
                    gosub_hurtK();
                    sound.PlaySound("spring");
                    world.Sprite[j].jumpM = world.Sprite[k].jumpM;
                    if (world.Sprite[k].hp <= 0) {
                        world.Sprite[k].proc->death_animation();
                    }
                }
            }
        }

        //Player illigetimately touches goomba.
        if (world.Sprite[j].kind == Kind::player
            && (world.Sprite[k].kind == Kind::goomba_thing || world.Sprite[k].kind == Kind::enemy_weak_to_jumping)) {
            if (world.Sprite[j].flickerTime < world.clock) {
                if (world.Sprite[j].z < world.Sprite[k].length
                    || world.Sprite[j].lastJump < world.Sprite[j].z) {
                    world.Sprite[j].hp = world.Sprite[j].hp - 1;
                    gosub_hurtj();
                    if (world.Sprite[j].hp <= 0) {
                        world.Sprite[j].proc->death_animation();
                    }
                }
            }
        }


        //Player jumps on bouncy object
        if (world.Sprite[k].kind == Kind::trampoline) {

            if (world.Sprite[j].kind == Kind::fireball || world.Sprite[j].kind == Kind::enemy_bullet) {
                goto overalready;
            }

            //BOUNCE TIME!
            if (world.Sprite[j].z > world.Sprite[k].length
                && world.Sprite[j].lastJump > world.Sprite[j].z) {
                world.Sprite[j].jumpStart = world.Sprite[j].z; //sends thing up into the air
                world.Sprite[j].jumpTime = world.clock; //sends thing up into the air, even if (goomba is flashing
                sound.PlaySound("spring");
                world.Sprite[k].mode = "bounce";
                world.Sprite[k].miscTime = world.clock + 1;
                world.Sprite[j].jumpM = world.Sprite[k].jumpM;
            }
            //OH CRAP! I NO BOUNCE
            if (world.Sprite[j].z < world.Sprite[k].length
                || world.Sprite[j].lastJump < world.Sprite[j].z) {
                if (world.Sprite[j].kind == Kind::unmoveable || world.Sprite[j].kind == Kind::fireball
                    || world.Sprite[j].kind == Kind::trampoline) {
                    goto britneyDog2;
                }
                world.Sprite[j].x = world.Sprite[j].lastX;
                world.Sprite[j].y = world.Sprite[j].lastY;
        britneyDog2:
                EMPTY_LABEL_HACK
            }
        overalready:
            EMPTY_LABEL_HACK
        }


        if (world.Sprite[j].kind == Kind::unmoveable) {
            if (world.Sprite[k].kind == Kind::unmoveable || world.Sprite[k].kind == Kind::fireball
                || world.Sprite[k].kind == Kind::trampoline || world.Sprite[k].kind == Kind::enemy_bullet) {
                goto britneyDog;
            }

            world.Sprite[k].x = world.Sprite[k].lastX;
            world.Sprite[k].y = world.Sprite[k].lastY;
        britneyDog:
            EMPTY_LABEL_HACK
        }

        if (redo == false) {
            redo = true; HolderJ = j; j = k; k = HolderJ; goto redo2;
        }
        if (redo == true) { HolderJ = j; j = k; k = HolderJ; }

        return;  // exit sub
    }

    int checkProx(const int who) {
       int theclosest = 0;
       int min = 0;

       min = 9999;
       theclosest = 0;
       for (int penguin = 0; penguin <= 2; ++ penguin) {
           if (!world.numberPlayers.player[penguin]) {
               continue;
           }

           // abs(x2-x1)^2+abs(y2-y1)^2
           const int buttcat = lp3::narrow<int>(
               std::sqrt(
                   std::pow(
                       std::abs(world.Sprite[penguin * 10].x
                                   - world.Sprite[who].x),
                       2)
                   +
                   std::pow(
                       std::abs(world.Sprite[penguin * 10].y
                                   - world.Sprite[who].y),
                       2)
               ));

           if (buttcat < min) {
               theclosest = penguin;
               min = buttcat;
           }
       }

       return theclosest * 10;
    }

    gsl::owner<GameProcess *> flipGame() {
        // I think this handles switching to different rooms or levels.
        int penguin;
        (void)penguin;  //2017- is this unused?

        if (this->exitS && boost::starts_with(world.screen, "Level")) {
            double sapple = boost::lexical_cast<double>(world.screen.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            if (sapple > 1.49) {
                return create_title_screen(context);
            }
            world.screen = str(boost::format("Level%s") % sapple);
            return new LegacyGame(context, std::move(world));
        } // End If

        //playWave "conzero.wav"
        if (world.screen == "title") {
            //playWave "conTen.wav"
            world.screen = "title2";
            return create_title_screen(context);
        }

        if (world.screen == "deadscreen") {
            world.screen = "title";
        }

        if (std::all_of(begin(world.player_data), end(world.player_data),
                        [](PlayerData & pd) { return !pd.active; }))
        {
            return create_gameover_screen(context);
        }

        //if (world.Sprite[0].name == "dead"
        //    && world.Sprite[10].name == "dead"
        //    && world.Sprite[20].name == "dead") {
        //  return create_gameover_screen(context);
        //}

        return nullptr;
    }

    void findPlayers() {
        std::array<bool, 3> active_players = {
            world.player_data[0].active,
            world.player_data[1].active,
            world.player_data[2].active
        };
        world.numberPlayers
            = ActivePlayers::find_from_active_players(active_players);
    }

    int findQ(const std::string & who) {
        int goatX = 0;

        for (std::size_t opera = 0; opera < world.Sprite.size(); ++ opera) {
            if (world.Sprite[opera].name == who) { goatX = goatX + 1; }
        }

        return goatX;
    }

    void goToLevel(const double which) {
        world.Gravity = 0;

        if (which == 1.1 || which == 1) {
            destroyEverything(world, view, sound);
            this->MakeLevel(1.0f, "Level1Opening.ogg", "Level1.cap",
// TSNOW: This is such a hack, but honestly the graphic for the apartment
//        carpet- which already looked terrible - makes the eyes bleed
//        when the texture is manually repeated to make a 64x64 bitmap.
//        It's so much worse I'd rather only use it in Emscripten.
#ifdef LP3_COMPILE_TARGET_EMSCRIPTEN
                            "Lv1bg.png",
#else
                            "Lv1bg.bmp",
#endif
                            10, 10,
                            true, true,
                            std::vector<glm::vec2>{
                                { 50, 220}
                            });

            world.camera.set_boundary({1010, 905 + 480});

            sound.LoadSound(16, "goomba.wav", "dying explosion");
            sound.LoadSound(17, "GoombaOuch.wav", "Goomba Ouch");
            sound.LoadSound(18, "KerboDie.wav", "Kerbose Die");
            sound.LoadSound(19, "KerbHurt.wav", "Kerbose Ouch");
            sound.LoadSound(20, "PutuloHurt.wav", "putulohurt");
            sound.LoadSound(21, "PutuloDie.wav", "putulodie");
        }

        if (which == 1.2) {
            this->MakeLevel(1.2f, "Level1.ogg", "level1b.cap", "Lv1bg2.png",
                            100, 100,
                            true, true,
                std::vector<glm::vec2>{
                    { 1122, 1650 }
            });
            world.camera.set_boundary({1194, 1900});
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "BS Death.wav", "Paul Shrink");
            sound.LoadSound(20, "WhaWhee.wav", "Stick Awaken");
        }

        if (which == 1.3) {
            this->MakeLevel(1.3f, "Level1.ogg", "level1c.cap", "lv1bg3.png",
                            10, 10,
                            false, false,
                std::vector<glm::vec2>{
                    { 242, 2000 },
                    { 42, 300 },
                    { 42, 300 }
            }
                );


            world.camera.set_boundary({1244, 2273});
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (which == 1.4) {
            this->MakeLevel(1.4f, "Level1.ogg", "level1d.cap", "level1birdstreet.png",
                            98, 480,
                false, false, std::vector<glm::vec2>{
                    { 42, 300 }
            });

            world.camera.set_boundary({3000, 480});
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (world.Gravity == 0) {
            // because Gravity is set to 20 in make level, we know that there
            // has been no other levels selected
            // in other words, somebody screwed up
            world.screen = "intro story"; // this will remind them that they failed
            // Note from present day: sure, just tell yourself that kid.
        }
    }

    long hitdetection(int num1, int num2, bool whatKind=false) {
        //TSNOW: I deleted a lot of commented code here as it was already pretty
        //       confusing.
        long result = 0;

        int con1 = 0;
        int con2 = 0;
        int con3 = 0;

        int left = 0;
        int right = 0;
        int up = 0;
        int down = 0;
        //Rem-checks to see if they hit from left or right
        con1 = 0; con2 = 0; con3 = 0;
        con1 = 0;
        if ((world.Sprite[num1].x + world.Sprite[num1].wide)
                >= world.Sprite[num2].x
            && world.Sprite[num1].x < world.Sprite[num2].x) {
            con1 = 1; left = 1;
        }
        if ((world.Sprite[num2].x + world.Sprite[num2].wide) >= world.Sprite[num1].x && world.Sprite[num2].x < world.Sprite[num1].x) {
            con1 = 1; right = 1;
        }

        //Rem- Checks to see if they ever collide from the top to bottom (Y)
        con2 = 0;

        if ((world.Sprite[num1].y + world.Sprite[num1].high)
                >= world.Sprite[num2].y
            && world.Sprite[num1].y < world.Sprite[num2].y) {
            con2 = 1; up = 1;
        }
        //Rem- Added as of 11/27/00
        if ((world.Sprite[num2].y + world.Sprite[num2].high)
                >= world.Sprite[num1].y
            && world.Sprite[num2].y < world.Sprite[num1].y) {
            con2 = 1; down = 1;
        }

        // 2017: unreferenced label:
        // screwthis2:
        if (world.Sprite[num1].kind == Kind::unmoveable || world.Sprite[num2].kind == Kind::unmoveable) {
            con3 = 1;
            goto screwthis3;
        }

        //Rem- The THIRD dimension, Z!

        if ((world.Sprite[num1].z + (world.Sprite[num1].length * 1.5))
                >= world.Sprite[num2].z
            && world.Sprite[num1].z < world.Sprite[num2].z) {
            con3 = 1;
        }
        if (world.Sprite[num1].z == world.Sprite[num2].z) { con3 = 1; }
        //Rem- Added as of 11/27/00
        if ((world.Sprite[num2].z + world.Sprite[num2].length * 1.5)
                >= world.Sprite[num1].z
            && world.Sprite[num2].z < world.Sprite[num1].z) {
            con3 = 1;
        }

        screwthis3:
        if (con1 == 1 && con2 == 1 && con3 == 1) { result = 1; }
        if (whatKind == true) {
            if (left == 1) { result = 2; }
            if (right == 1) { result = 3; }
            if (up == 1) { result = 4; }
            if (down == 1) { result = 5; }
            if (left == 1 && up == 1) { result = 6; }
            if (left == 1 && down == 1) { result = 7; }
            if (right == 1 && up == 1) { result = 8; }
            if (right == 1 && down == 1) { result = 9; }
        }

        return result;
    }


    void initPlayers() {
        /*for (k = 0; k <= 2; ++ k) {
            if (world.player_data[k].playerName == "") { world.player_data[k].playerName = "redead"; }
        }*/


        //Rem****************88
        //After killing the players above by setting the sprites (bodies) to "dead",
        //it reincarnates them here by calling createPlayer
        //Rem*************************************8

        view.LoadTexture(0, "System.png", 336, 397);

        //Rem******************************************************8
        //          THIS PART LOADS UP WEAPONS
        //Rem******************************************************8

        /*for (int index = 0; index < 3; ++index) {
            if (world.numberPlayers.player[index]) {
                world.Sprite[index * 10].name = world.player_data[0].playerName;
                for (int k = index * 10 + 1; k <= (index * 10) + 9; ++k) {
                    world.Sprite[k].x = 60;
                    world.Sprite[k].y = 70;
                    world.Sprite[k].name = "fireball";
                    if (world.player_data[index].playerName == "Thomas"
                        || world.player_data[index].playerName == "Nick") {
                        world.Sprite[k].name = "fireball";
                    }
                    if (world.player_data[index].playerName == "Nicky") {
                        world.Sprite[k].name = "bomb";
                    }
                }
            }
        }*/

        //Rem- THIS PART MAKES SPRITES DIFFERENT COLORS
        /*if (world.Sprite[0].name == world.Sprite[10].name) {
            world.Sprite[10].color = view::qb_color(10);
        }*/

        sound.LoadSound(15, "Spring.wav", "spring");
    }

    void killLimit(int jex) {
        // Rem- Kills unruly sprites who are out of bounds
        auto & s = world.Sprite[jex];
        if (s.x > world.camera.boundary().x || s.x < -10) { kill(s); }

        if (s.y > world.camera.boundary().x || s.y < -10) { kill(s); }
    }

    void loadAnimation(int who, const std::string & file) {
        auto & s = world.Sprite[who];
        view.load_animation_file(s.Aframe, file);
    }

    //TSNOW: This looks insane. It looks as if there was too much in PlayGame,
    //       so I made this function to get called for each level... which
    //       also over time would have grown to have been too much.
    void levelR(const int which, const int who) {
        int k = 0;

        auto & ws = world.Sprite[who]; // with sprite

        //LEVEL 1****************************************************************
        if (which == 1) {  //LEVEL 1 ****************************

            //If .name = "bullet" Then

            //End If
            if (ws.name == "greenspring") {
                if (ws.mode == "bounce") {
                    ws.frame = ws.frame + 1;
                    if (ws.frame > 5) { ws.frame = 2; }
                    if (ws.miscTime < world.clock) { ws.mode = ""; ws.frame = 1; }
                }
            }


            if (ws.name == "clouds") {
                ws.srcx = ws.srcx + (fs_speed_factor * 0.5);
                ws.srcx2 = ws.srcx2 + (fs_speed_factor * 0.5);
                ws.Aframe[1].x = ws.Aframe[1].x + 1;
                ws.Aframe[1].x2 = ws.Aframe[1].x2 + 1;
            }

            if (ws.name == "bullet") {     ////This is a strange type of bullet that in retrospect feels more like a bubble
                //if (ws.seekx <> -1) then

                this->killLimit(who);
                {
                    off_camera_kill(ws, world.camera);
                }


                //if (ws.mode = "") then


                while(!(
                    (ws.seekx > world.camera.boundary().x || ws.seekx < 0)
                        || (ws.seeky > world.camera.boundary().y || ws.seeky < 0)
                    )) {
                    if (ws.seekx > ws.x) {
                        ws.seekx = ws.seekx + ((ws.seekx - ws.x));
                    }
                    if (ws.seekx < ws.x) {
                    ws.seekx = ws.seekx - ((ws.x - ws.seekx));
                    }

                    if (ws.seeky > ws.y) {
                        ws.seeky = ws.seeky + ((ws.seeky - ws.y));
                    }
                    if (ws.seeky < ws.y) {
                        ws.seeky = ws.seeky - ((ws.y - ws.seeky));
                    }
                }

                this->seeker(who);
                ws.frame = ws.frame + 1; if (ws.frame > 1) { ws.frame = 0; }
            }

            if (ws.name == "paulrun") {
                if (ws.mode == "") { ws.mode = "right"; }

                if (ws.mode == "right") {
                    ws.reverse = false;
                    ws.x = ws.x + fs_speed_factor;
                    ws.seekx = ws.seekx + fs_speed_factor;
                    if (ws.seekx > 100) {
                        ws.mode = "left"; ws.seekx = 0; ws.dir = ""; }
                }
                if (ws.mode == "left") {
                    ws.reverse = true;
                    ws.x = ws.x - fs_speed_factor;
                    ws.seekx = ws.seekx + fs_speed_factor;
                    if (ws.seekx > 100) {
                        ws.mode = "right"; ws.seekx = 0; ws.dir = ""; }
                }

                if (ws.seekx >= 50 && ws.dir != "done") {
                    this->shoot(who, "paulbullet",
                                world.Sprite[checkProx(who)].x,
                                world.Sprite[checkProx(who)].y);
                    ws.dir = "done";
                }
            }

            if (ws.name == "bluestick") {
                k = random.next() * 2 + 1;
                if (k == 1) { ws.x = ws.x + fs_speed_factor; }
                if (k == 2) { ws.x = ws.x - fs_speed_factor; }
                k = random.next() * 2 + 1;
                if (k == 1) { ws.y = ws.y + fs_speed_factor; }
                if (k == 2) { ws.y = ws.y - fs_speed_factor; }
                k = random.next() * 20 + 1;
                if (k == 1) { if (ws.z == 0) {
                    ws.jumpStart = ws.z; ws.jumpTime = world.clock; } }
            }

            if (ws.name == "pigeonbomber") {
                ws.z = ws.z + fs_speed_factor;
                //ws.frame = ws.frame + 1
                //if (ws.frame > 2) then ws.frame = 1

                this->seeker(who);
                if (ws.x < 1) { ws.x = world.camera.boundary().x; }

                if (ws.miscTime < world.clock) {
                    this->shoot(who, "bluestick",
                                world.Sprite[checkProx(who)].x,
                                world.Sprite[checkProx(who)].y);
                    ws.miscTime = world.clock + 2;
                }
            }
            return;
        }
    }

    void loadLevel(const std::string & file) {
        int j = 0;
        std::array<std::string, 10> texFile;
        std::array<int, 10> texwide;
        std::array<int, 10> texhigh;
        {
            auto f = vb.OpenForInput(file);
            for (j = 0; j <= 9; ++ j) {
                if (j > 3 || j < 1) {
                    f.input(texFile[j], texwide[j], texhigh[j]);
                    view.LoadTexture(j, texFile[j], texwide[j], texhigh[j]);
                }
            }
            for (j = 40; j <= 100; ++j) {
                auto & s = world.Sprite[j];
                f.input(s.name, s.x, s.y, s.z, s.srcx, s.srcy, s.srcx2,
                        s.srcy2, s.wide, s.high, s.length, s.texture, s.visible,
                        s.kind, s.zOrder);
            }
        }
    }

    void MakeLevel(const float stage,
                   const std::string & lvlBgMusic, const std::string & levelFile,
        const std::string & levelBgFile, const int lvlBgWidth,
        const int lvlBgHeight,
        const bool stopMusic,
        const bool,  // loadScreen
        std::vector<glm::vec2> player_spawn_locations) {

        destroyEverything(world, view, sound, 2);

        world.camera.set_size({640, 480});

        if (stopMusic == true) { sound.PlayBgm(""); }


        this->loadLevel(levelFile); //"Level1b.cap"

        world.Gravity = 20;

        int j = 0;

        world.camera.focus({0, 0});
        view.LoadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight);

        this->initPlayers();

        CharacterProcEnv env{ context, random, world.clock, world.camera };

        // First 30 sprites were for player stuff (10 each)
        for (auto & pd : world.player_data) {
            const auto & loc = lp3::narrow<int>(player_spawn_locations.size()) > pd.index
                ? player_spawn_locations[pd.index]
                : player_spawn_locations.back();
            if (pd.active) {
                proc_manager.add_process(
                    proc::create_player_proc(env, world.game_state, pd,
                                             entity_manager, loc));
            }
        }
        ///*for (int h = 0; h < 3; ++h) {
        //  proc_manager.add_process(
        //      legacy_add_process(env, world, entity_manager, h*10,
        //                         world.Sprite[h*10], world.Sprite[h*10].name));
        //}*/

        // old school sprites 30-40 reserved for cinematics:
        proc_manager.add_process(
            proc::create_cinema_proc(env, entity_manager, stage));

        entity_manager.skip_to(40);

        // old school sprites >=40 for things loaded out of the level
        for (j = 40; j < lp3::narrow<int>(world.Sprite.size()); ++j) {
            // First, try to create a new style proc:
            auto * proc =
                proc::create_enemy_proc(env, entity_manager, world.Sprite[j].name);
            // If that fails, use the old nasty proc stuff
            if (!proc) {
                proc = legacy_add_process(
                    env, world, entity_manager, j,
                    world.Sprite[j], world.Sprite[j].name);
                entity_manager.grab_sprite(); // make sure the manager doesn't
                                              // use this next sprite.
            }
            proc_manager.add_process(proc);
        }

        if (stopMusic == true) { sound.PlayBgm(lvlBgMusic); }

        this->exitS = false;
    }



    void seeker(int who) {
        auto & s = world.Sprite[who];
        if (s.x < s.seekx) { s.x = s.x + (s.mph * fs_speed_factor); }
        if (s.x > s.seekx) { s.x = s.x - (s.mph * fs_speed_factor); }
        if (s.y < s.seeky) { s.y = s.y + (s.mph * fs_speed_factor); }
        if (s.y > s.seeky) { s.y = s.y - (s.mph * fs_speed_factor); }
    }


    void shoot(int who, const std::string & what, int wherex, int wherey) {
        int opera;

        for (opera = (who + 1); opera < lp3::narrow<int>(world.Sprite.size()); ++ opera) {
            if (world.Sprite[opera].name == "" || world.Sprite[opera].name == "empty" || world.Sprite[opera].name == "dead") {
                // killS opera
                world.Sprite[opera].name = what;
                break;
            }
        }

        if (opera >= 95) { return; }  //2017: WAT?

        world.Sprite[opera].trueVisible = 0;
        world.Sprite[opera].visible = true;
        world.Sprite[opera].flickOn = false;
        world.Sprite[opera].texture = world.Sprite[who].texture;
        world.Sprite[opera].wide = world.Sprite[who].wide;
        world.Sprite[opera].high = world.Sprite[who].high;

        world.Sprite[who].proc->spawn(world.Sprite[opera], what);
        world.Sprite[opera].zOrder = -1;
        world.Sprite[opera].x = world.Sprite[who].x;
        world.Sprite[opera].y = world.Sprite[who].y;
        world.Sprite[opera].z = world.Sprite[who].z;
        world.Sprite[opera].seekx = wherex;
        world.Sprite[opera].seeky = wherey;
    }

};  // end of GameImpl class


gsl::owner<GameProcess *> create_legacy_screen(
    GameContext context,
    World && world,
    std::array<boost::optional<std::string>, 3>)
{
    //TODO: Use players, somehow
    return create_now_loading_screen(
        context,
        [world(std::move(world))](GameContext context_2) mutable {
        return new LegacyGame(context_2, std::move(world));
    });

}

}   }  // end namespace
