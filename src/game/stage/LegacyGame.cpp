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
#include "Level.hpp"
#include "../GameOverScreen.hpp"
#include "../NowLoading.hpp"
#include "../TitleScreen.hpp"

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
    GameState game_state;
    World world;
    Random random;
    std::int64_t animation_timer;
    EntityManager entity_manager;
    CharacterProcManager proc_manager;
    // This handles exitting to the next screen.
    bool exitS;
    std::string screen_name;
    double gravity;
    double clock;
    std::vector<InputReceivingCharacterProc *> player_procs;

public:
    LegacyGame(GameContext _context,
               GameState && game_state_arg,
               const std::string & _screen_name)
    :   context(_context),
        vb(context.media),
        view(context.view),
        sound(context.sound),
        game_state(game_state_arg),
        world(),
        random(),
        animation_timer(0),
        entity_manager(world),
        proc_manager(),
        exitS(false),
        screen_name(_screen_name),
        gravity(0),
        clock(0)
    {
        LP3_ASSERT(boost::starts_with(this->screen_name, "Level"));

        sound.silence_sfx();

        std::string crapple = this->screen_name.substr(5);
        double boogeycrap = boost::lexical_cast<double>(crapple);

        this->goToLevel(boogeycrap);
    }

    void handle_input(const input::Event & event) override {
        switch(event.key) {
            case input::Key::up:
            case input::Key::down:
            case input::Key::left:
            case input::Key::right:
            case input::Key::attack:
            case input::Key::jump:
            case input::Key::power_up:
                if (player_procs[event.player]) {
                    player_procs[event.player]->handle_input(event);
                }
                break;
            case input::Key::skip_scene:
                if (event.value) {
                    this->exitS = true; sound.PlayWave("FDis.wav");
                }
                break;
            case input::Key::lemon_time:
                // LemonTime no longer working.
                // it was a cheat code that made the players small, like
                // "reduce" from Graidus, and their fireballs big.
                // I wonder when I broke it?
                break;
            default:
                LP3_ASSERT(false);
        }
    }

    gsl::owner<GameProcess *> update() override {
        // The old code used a double for the clock. This maps to the
        // current hard coded ms_per_update.
        this->clock += fs_s_per_update;

        this->findPlayers();

        if (game_state.numberPlayers.any_player_active()) {
            auto focus_point = world.find_middle_of_group(Kind::player);
            if (focus_point) {
                world.camera.focus(*focus_point);
            }
        }


        //-----------------------------------------------------------
        //START OF NORMAL ROUTINE
        //------------------------------------------------------------

        // 2017: todo: try making these only appear in the loops that need them
        int j;

        flicker(clock, world);

        //Rem-------------------------------------------------------------------
        //               THIS SECTION DOES THE JUMPING
        //Rem-------------------------------------------------------------------

        for (j = 0; j < lp3::narrow<int>(world.Sprite.size()); ++j) {
            world.Sprite[j].update_jump_physics(gravity);
        }


        // CharacterProcEnv env{ context, random, this->clock, camera };
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
                    CharacterProcEnv env{ context, random, this->clock, world.camera };

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
                    if (touching(s, world.Sprite[penguin * 10])
                        && world.Sprite[penguin].name
                        == game_state.player_data[penguin].playerName) {
                        this->exitS = true;
                    }
                }
            } else if (s.proc) {
                //CharacterProcEnv env{context, random, this->clock};
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

        entity_manager.run_collision_detection(sound, this->clock);

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

    gsl::owner<GameProcess *> flipGame() {
        // I think this handles switching to different rooms or levels.
        int penguin;
        (void)penguin;  //2017- is this unused?

        if (this->exitS && boost::starts_with(this->screen_name, "Level")) {
            double sapple = boost::lexical_cast<double>(this->screen_name.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            if (sapple > 1.49) {
                return create_title_screen(context);
            }
            this->screen_name = str(boost::format("Level%s") % sapple);
            return new LegacyGame(
                context, std::move(game_state), this->screen_name);
        } // End If

        //playWave "conzero.wav"
        if (this->screen_name == "title") {
            //playWave "conTen.wav"
            this->screen_name = "title2";
            return create_title_screen(context);
        }

        if (this->screen_name == "deadscreen") {
            this->screen_name = "title";
        }

        if (std::all_of(begin(game_state.player_data),
                        end(game_state.player_data),
                        [](PlayerData & pd) { return !pd.active; }))
        {
            return create_gameover_screen(context);
        }

        return nullptr;
    }

    void findPlayers() {
        std::array<bool, 3> active_players = {
            game_state.player_data[0].active,
            game_state.player_data[1].active,
            game_state.player_data[2].active
        };
        game_state.numberPlayers
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
        // TODO:
        //      ok, after mucking with this code for a bit here's my idea.
        //      Change the actual special level loading logic to do this:
        //
        //      Create a magical process- this can load sound, change the
        //      textures, do anything else crazy. But it gets added as a
        //      typical process that's run with everything else. This opens
        //      the door in the first room.
        //
        //      Second: either return a list of sprites thats from level data,
        //      or make a function that does all that crap callable. Currently
        //      such a function is make_level, but you have to send it the
        //      entire universe. Hopefully in the future this can be scaled
        //      back.
        //
        //      If I had a callable to load that stuff, I might want one to
        //      load players too. At this point I'm feeling like this - maybe
        //      - could be a subclass sandbox.
        //


        // 2018-06 - this part is problematic and I want to make sure it's only
        //           called once per LegacyGame instance.
        LP3_ASSERT(this->gravity == 0);

        destroyEverything(world, view, sound, 2);

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
                            true,
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
                            true,
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
                            false,
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
                false, std::vector<glm::vec2>{
                    { 42, 300 }
            });

            world.camera.set_boundary({3000, 480});
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (this->gravity == 0) {
            // because Gravity is set to 20 in make level, we know that there
            // has been no other levels selected
            // in other words, somebody screwed up
            this->screen_name = "intro story"; // this will remind them that they failed
            // Note from present day: sure, just tell yourself that kid.
        }

        this->exitS = false;
    }

    void MakeLevel(const float stage,
                   const std::string & lvlBgMusic, const std::string & levelFile,
        const std::string & levelBgFile, const int lvlBgWidth,
        const int lvlBgHeight,
        const bool stopMusic,
        std::vector<glm::vec2> player_spawn_locations) {
        auto result = make_level(context, world, random, game_state, clock, entity_manager, proc_manager, stage, player_spawn_locations, lvlBgMusic, levelFile, levelBgFile, lvlBgWidth, lvlBgHeight, stopMusic);
        this->gravity = result.gravity;
        this->player_procs = result.player_procs;
    }

    struct NewLevelInfo {
        double gravity;
        std::vector<InputReceivingCharacterProc *> player_procs;
    };

    static NewLevelInfo make_level(
            GameContext & context,
            World & world,  // with more work, this can go away
            Random & random,  // used just for CharacterProcEnv
            GameState & game_state,
            const double & clock,  // dito
            EntityManager & entity_manager,
            CharacterProcManager & proc_manager,
            const float stage,
            std::vector<glm::vec2> player_spawn_locations,
            const std::string & lvlBgMusic, const std::string & levelFile,
            const std::string & levelBgFile, const int lvlBgWidth,
            const int lvlBgHeight,
            const bool stopMusic)
    {
        NewLevelInfo result;
        result.gravity = 20;



        world.camera.set_size({640, 480});

        if (stopMusic == true) { context.sound.PlayBgm(""); }

        // this->loadLevel(levelFile); //"Level1b.cap"

        Vb vb(context.media);
        FSLevelFile level_data = load_fs_level(vb, levelFile);

        // Load textures
        for (const auto & texture : level_data.textures) {
            context.view.LoadTexture(texture.index, texture.name,
                             texture.size.x, texture.size.y);
        }


        world.camera.focus({0, 0});
        context.view.LoadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight);

        context.view.LoadTexture(0, "System.png", 336, 397);

        context.sound.LoadSound(15, "Spring.wav", "spring");


        CharacterProcEnv env{ context, random, clock, world.camera };

         // First 30 sprites were for player stuff (10 each)
        for (auto & pd : game_state.player_data) {
            const auto & loc = lp3::narrow<int>(player_spawn_locations.size()) > pd.index
                ? player_spawn_locations[pd.index]
                : player_spawn_locations.back();
            if (pd.active) {
                std::unique_ptr<InputReceivingCharacterProc> player_proc(
                    proc::create_player_proc(env, game_state, pd,
                                             entity_manager, loc));
                result.player_procs.push_back(player_proc.get());
                proc_manager.add_process(player_proc.release());
            }
        }

        // old school sprites 30-40 reserved for cinematics:
        proc_manager.add_process(
            proc::create_cinema_proc(env, entity_manager, stage));

        const int arby_offset = 40;

        entity_manager.skip_to(arby_offset);

        // old school sprites >=arby_offset for things loaded out of the level
        for (int j = arby_offset; j < lp3::narrow<int>(world.Sprite.size()); ++j) {
            if (lp3::narrow<int>(level_data.sprites.size()) > (j - arby_offset)) {
                const auto & sprite_level_data = level_data.sprites[j - arby_offset];

                // First, try to create a new style proc:
                auto * proc =
                    proc::create_enemy_proc(
                        env,
                        entity_manager,
                        sprite_level_data.name,
                        sprite_level_data.position,
                        sprite_level_data.texture.value);
                // If that fails, use the old nasty proc stuff
                if (!proc) {
                    proc = legacy_add_process(
                        env,
                        world,
                        entity_manager,
                        sprite_level_data.name,
                        sprite_level_data);
                }
                proc_manager.add_process(proc);
            }
        }

        if (stopMusic == true) { context.sound.PlayBgm(lvlBgMusic); }

        return result;
    }



};  // end of GameImpl class


gsl::owner<GameProcess *> create_legacy_screen(
    GameContext context,
    GameState && game_state,
    std::array<boost::optional<std::string>, 3>,
    const std::string & screen_name)
{
    //TODO: Use players, somehow
    return create_now_loading_screen(
        context,
        [game_state(std::move(game_state)), screen_name](
            GameContext context_2) mutable {
        return new LegacyGame(context_2, std::move(game_state), screen_name);
    });

}

namespace {
    gsl::owner<GameProcess *> create_legacy_screen_2(GameContext context) {
        GameState game_state;
        // This duplicates code from the player select class.
        // Put this somewhere else.
        game_state.numberPlayers = ActivePlayers::ap0();
        for (auto & pd : game_state.player_data) {
            pd.lives = 3;
        }
        game_state.player_data[0].active = true;
        game_state.player_data[0].playerName = "Thomas";

        std::string screen("Level1.1");
        std::array<boost::optional<std::string>, 3> _what_is_this;
        return create_legacy_screen(
            context, std::move(game_state), _what_is_this, screen);
    }

    RegisterGameProcess reg(
        "legacy-screen", "The actual game", create_legacy_screen_2);
}
}   }  // end namespace
