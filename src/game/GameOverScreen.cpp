#include "GameOverScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "BaseScreen.hpp"
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


class GameOverScreen : public GameProcess
{
private:
    Vb & vb;
    view::View & view;
    Sound & sound;
    World & world;
    Random random;

public:
    GameOverScreen(GameProcessSpace & space, view::View & view_arg,
               Sound & sound_arg, Vb & vb_arg, World & world_arg)
    :   GameProcess(space),
        vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
        world(world_arg),
        random()
    {
		sound.silence_sfx();
        world = World{};
        sound.PlayBgm("");
        world.screen = "gameOver";
        destroyEverything(world, view, sound);
        view.LoadTexture(0, "GameOver.png", 320, 287);
        {
            auto & s = world.Sprite[0];
            s.srcx = 1; s.srcy = 1; s.srcx2 = 320; s.srcy2 = 240;
            s.x = 0; s.y = 0; s.wide = 640; s.high = 480; s.visible = true;
            s.trueVisible = 1;
            s.name = "GameOverCloudBg";
            s.texture = 0;
            s.color = normColor;
        }
        {
            auto & s = world.Sprite[1];
            s.srcx = 1; s.srcy = 243; s.srcx2 = 320; s.srcy2 = 287;
            s.x = 0; s.y = 180; s.wide = 640; s.high = 94; s.visible = true;
            s.trueVisible = 1;
            s.name = "GameOverCloudTitle"; s.texture = 0;
            s.miscTime = world.clock + 4;
        }
        sound.PlayWave("GameOver.wav");

        findOrder(world);
    }

    void handle_input(const input::Event &) override {
    }

    void update() override {
        world.lasttime = world.clock + 3.33333333333333E-02;
        int j = 0;
        int k = 0;
        int penguin = 0;

		flicker(world);

        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];			

            if (s.name == "GameOverCloudTitle") {
                if (world.clock > s.miscTime) {
                    s.high = s.high + 2 * world.sFactor;
                    s.y = s.y - world.sFactor;
                    if (s.y < 0) { s.flickerTime = world.clock + 1; }
                    if (s.y < -300) {
                        s.wide = s.wide - (10 * world.sFactor);
                        s.x = s.x + 5 * world.sFactor;
                        if (s.wide < 0) {
                            s.visible = false;
							this->exec(create_title_screen(get_process_space(), view, sound, vb, world));
							return;
                        }
                    }
                }
            }
        }

        view.UpdateSprites();

        if (world.exitS == "true" && boost::starts_with(world.screen, "Level")) {
            double sapple = boost::lexical_cast<double>(world.screen.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            world.screen = str(boost::format("Level%s") % sapple);
        } // End If

		world.gpRate2 = world.gpRate2 + 1;
		if (world.clock > world.cranBerry2) {
			world.cranBerry2 = (world.clock + 1);
			world.gpRate = world.gpRate2;
			world.gpRate2 = 0;
		}
		for (j = 0; j < world.spritesInUse; ++j) {
			world.Sprite[j].lastX = world.Sprite[j].x;
			world.Sprite[j].lastY = world.Sprite[j].y;
		}
    }

private:
    
    void loadAnimation(int who, const std::string & file) {
        auto & s = world.Sprite[who];
        view.load_animation_file(s.Aframe, file);
    }


};  // end of GameImpl class


gsl::owner<GameProcess *> create_gameover_screen(
    GameProcessSpace & space,
    view::View & view, Sound & sound, Vb & vb, World & world)
{
    return new GameOverScreen(space, view, sound, vb, world);
}

}   }  // end namespace
