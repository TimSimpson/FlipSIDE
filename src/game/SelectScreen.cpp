#include "SelectScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "BaseScreen.hpp"
#include "CharacterProc.hpp"
#include "LegacyGame.hpp"

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


class SelectScreen : public GameProcess
{
private:
    Vb & vb;
    view::View & view;
    Sound & sound;
    World & world;
    Random random;

public:
    SelectScreen(GameProcessSpace & _space, view::View & view_arg,
               Sound & sound_arg, Vb & vb_arg, World & world_arg,
               std::array<bool, 3> keys_pressed)
    :   GameProcess(_space),
        vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
        world(world_arg),
        random()
    {
        sound.silence_sfx();

        // the select player screen
        world = World{};
		destroyEverything(world, view, sound);
        world.screen = "SelectPlayerz";
        for (int j = 0; j <= 2; ++j) {
            world.player_data[j].lives = 3;
            world.continues = 2;
        }

        this->NowLoading();
        view.UpdateSprites();
        view.LoadTexture(0, "PlayerS2.png", 320, 400);
        view.LoadTexture(-1, "PlayerS.png", 320, 240);
        world.camera.CameraWidth = 320;
        world.camera.CameraHeight = 240;

        {
            auto & s = world.Sprite[0];
            s.x = 2 * 2;
            s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (keys_pressed[0]) { s.visible = true; }
            s.name = "Selecter";
            s.frame = 1;
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[1];
            s.x = 106 * 2;
            s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (keys_pressed[1] == 1) { s.visible = true; };
            s.name = "Selecter";
            s.frame = 2;
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[2];
            s.x = 212 * 2; s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (keys_pressed[2]) { s.visible = true; };
            s.name = "Selecter";
            s.miscTime = world.clock + 2;
            s.frame = 3;
        }
        this->loadAnimation(0, "Selector.ani");
        this->loadAnimation(1, "Selector.ani");
        this->loadAnimation(2, "Selector.ani");

        sound.PlayBgm("Player SelectWAV.wav");
        sound.PlayWave("Select Your Characters of Justice.wav");
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
                    world.exitS = "true"; sound.PlayWave("FDis.wav");
                }
                break;
            case input::Key::power_up:
                world.player_data[0].slicer = true;
                world.player_data[0].GradeUp = 2;
                world.Sprite[0].wide = 25;
                world.Sprite[0].high = 25;
                sound.PlayWave("SoupedUp.wav");
                break;
            case input::Key::lemon_time:
                world.LemonTime = true;
                break;
            default:
                LP3_ASSERT(false);
        }
    }

    void update() override {
        world.lasttime = world.clock + 3.33333333333333E-02;
        int j = 0;
        int k = 0;

        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];

		    if (s.name == "Selecter") {
                if (world.clock > s.miscTime) {
                    s.miscTime = world.clock + 0.1;  //this way, the person won't scream through the selection box because it moves at 40 FPS!
                    //For k = 0 To 2
                    k = j;

                    if (world.player_data[k].upKey == true) {
                        s.frame = s.frame - 1;
                        if (s.frame == 0) { s.frame = 5; }
                    }
                    if (world.player_data[k].DownKEY == true) {
                        s.frame = s.frame + 1;
                        if (s.frame == 6) { s.frame = 1; }
                    }
                    if (world.player_data[k].AttackKey == true
                        || world.player_data[k].JumpKey == true) {
                        s.mode = "done";
                        s.name = "KukoFax";
                        if (s.frame == 1) { sound.PlayWave("PickTom.wav"); }
                        if (s.frame == 2) { sound.PlayWave("PickNick.wav"); }
                        if (s.frame == 3) { sound.PlayWave("PickDrew.wav"); }
                        if (s.frame == 5) { sound.PlayWave("PickNicky.wav"); }
                    }
                //Next k
                }
            }
        }

        view.UpdateSprites();

		if (world.exitS == "true" && boost::starts_with(world.screen, "Level")) {
			double sapple = boost::lexical_cast<double>(world.screen.substr(5));
			sapple = sapple + 0.1; // WTF, right? It's in the original code though...
			world.screen = str(boost::format("Level%s") % sapple);
		} // End If

		if (world.screen == "SelectPlayerz") {
			this->selectPlayerS();
		}
    }

private:

    void loadAnimation(int who, const std::string & file) {
        auto & s = world.Sprite[who];
        view.load_animation_file(s.Aframe, file);
    }



    void NowLoading() {
        view.LoadTexture(-1, "NowLoading.png", 320, 240);
        world.camera.CameraWidth = 320;
        world.camera.CameraHeight = 240;
        view.UpdateSprites();
        //Call DrawStuff
        world.camera.CameraWidth = 640;
        world.camera.CameraHeight = 480;
    }



    bool selectPlayerS() {
        // after you all select players, it gets up the results

        if (world.Sprite[0].mode == "done" || world.Sprite[0].visible == false) {
            if (world.Sprite[1].mode == "done" || world.Sprite[1].visible == false) {
                if (world.Sprite[2].mode == "done" || world.Sprite[2].visible == false) {
                    for (int i = 0; i <= 2; ++i) {
                        if (world.Sprite[i].visible == false) {
                            world.Sprite[i].mode = "";
                        }
                    }
                    if (world.Sprite[0].mode == "done") {
                        world.numberPlayers = 1; }
                    if (world.Sprite[1].mode == "done") {
                        world.numberPlayers = 4; }
                    if (world.Sprite[2].mode == "done") {
                        world.numberPlayers = 5; }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[1].mode == "done") {
                        world.numberPlayers = 2;
                    }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 6;
                    }
                    if (world.Sprite[1].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 7;
                    }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[1].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 3;
                    }

					std::array<boost::optional<std::string>, 3> players;
                    for (int penguin = 0; penguin <= 2; ++ penguin) {
                        auto & s = world.Sprite[penguin];
                        //Next penguin
                        if (s.frame == 1) {
                            world.player_data[penguin].playerName = "Thomas";
							players[penguin] = "Thomas";
						}
                        if (s.frame == 2) {
                            world.player_data[penguin].playerName = "Nick";
							players[penguin] = "Nick";
						}
                        if (s.frame == 3) {
                            world.player_data[penguin].playerName = "Andrew";
							players[penguin] = "Andrew";
						}
                        if (s.frame == 4) {
                            world.player_data[penguin].playerName = "Phil";
							players[penguin] = "Phil";
						}
                        if (s.frame == 5) {
                            world.player_data[penguin].playerName = "Nicky";
							players[penguin] = "Nicky";
						}
                    }
                    //1 Only player 1
                    //2 Player 1 and 2
                    //3 All three Players
                    //4 Just player 2
                    //5 Just player 3
                    //6 Players 1 and 3
                    //7 Players 2 and 3

                    world.screen = "Level1.1";
					this->exec(
						create_legacy_screen(this->get_process_space(),
						view, sound, vb, world, players));
					return true;
                }
            }
        }
		return false;
    }


};  // end of GameImpl class


gsl::owner<GameProcess *> create_select_screen(
    GameProcessSpace & space,
    view::View & view, Sound & sound, Vb & vb, World & world,
    std::array<bool, 3> keys_pressed)
{
    return new SelectScreen(space, view, sound, vb, world, keys_pressed);
}

}   }  // end namespace
