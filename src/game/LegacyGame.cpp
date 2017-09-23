#include "LegacyGame.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "BaseScreen.hpp"
#include "CharacterProc.hpp"
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
	Camera camera;

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
		camera(world.camera)
    {
		LP3_ASSERT(boost::starts_with(world.screen, "Level"));
		LP3_ASSERT(world.currentScreen != world.screen);

		sound.silence_sfx();

        //as long as this is set to this crazy value, it won't load it again.
        world.currentScreen = world.screen;

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

    gsl::owner<GameProcess *> update() override {
		set_time_stuff(world);

        int focus_x = 0;
		int focus_y = 0;


                     //                   CAMERA TIME
                     //----------------------------------------------------------------------
        this->findPlayers();
        //If Sprite(0).name <> playerName(0) And Sprite(10).name <> playerName(1) And Sprite(20).name <> playerName(2) Then gotFocus = -6

		//2017: Have no idea what the hell any of this means but it's how
		// these numbers used to map.
		//world.gotFocus = world.numberPlayers.focus.gotFocus;
		//int j = world.numberPlayers.focus.j;
		//int k = world.numberPlayers.focus.k;

        //1 Only player 1
        //2 Player 1 and 2
        //3 All three Players
        //4 Just player 2
        //5 Just player 3
        //6 Players 1 and 3
        //7 Players 2 and 3

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


			world.camera.CameraX = focus_x - (world.camera.CameraWidth / 2);
			world.camera.CameraY = focus_y - (world.camera.CameraHeight / 2);

			if (world.camera.CameraX < 1) { world.camera.CameraX = 1; }
			if (world.camera.CameraX + world.camera.CameraWidth >= world.camera.cameraStopX) {
				world.camera.CameraX = world.camera.cameraStopX - 1 - world.camera.CameraWidth;
			}
			if (world.camera.CameraY < 1) { world.camera.CameraY = 1; }
			if (world.camera.CameraY + world.camera.CameraHeight >= world.camera.cameraStopY) {
				world.camera.CameraY = world.camera.cameraStopY - 1 - world.camera.CameraHeight;
			}
		}


        int penguin = 0;
        int trueorg = 0; //focus_x and true org are buddies and also junk variables

        //-----------------------------------------------------------
        //START OF NORMAL ROUTINE
        //------------------------------------------------------------

		// 2017: todo: try making these only appear in the loops that need them
		int j;
		int k;





        //Rem-FLICKER-
        for (j = 0; j < world.spritesInUse; ++j) {

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
        //The ancient key from the past?!!
        //crapple = clock - jumptime(j)
        //z(j) = jumpstart(j) + (jumpstrength(j) * crapple) - (gravity * (crapple ^ 2))
        //If z(j) < 0 Then z(j) = 0: jumptime(j) = 0

        for (j = 0; j < world.spritesInUse; ++j) {
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
                if (s.z > 0) { s.z = s.z - world.sFactor; }
            }
        }



        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];
			// Handle all "level" stuff here, but call update on gameproc
			// otherwise.
			if (s.name == "script") {
				this->script();
			}  else	if (s.name == "frontdoor") {
				if (this->findQ("Kerbose") < 1) {
					kill(s);
					s.name = "exit";
					this->cinemaM(2);
				}
			} else if (s.proc) {
				//CharacterProcEnv env{context, random, world.clock};
				//s.proc->update(env, s, j, world);
			}

        }

		CharacterProcEnv env{ context, random, world.clock, camera };
		proc_manager.update();
        //TSNOW: end of the emulated with statement that creates variable "s",
        //       along with the for loop that used the "j" variable.
        //       Holy crap, after formatting that it's 1259 lines long.
        //       In Visual Basic it was 910 lines long.

        //---------------------------------------------------------------------
        //      END OF AN ERA
        //---------------------------------------------------------------------

        for (j = 0; j <= world.spritesInUse; ++ j) {
            //If Sprite(j).mover = True Then
            for (k = j + 1; k <= world.spritesInUse; ++k) {
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

        for (j = 0; j < world.spritesInUse; ++ j) {
            world.Sprite[j].lastX = world.Sprite[j].x;
            world.Sprite[j].lastY = world.Sprite[j].y;
        }
		animate();
		create_billboards(world, view.billboards());

		return nullptr;
    }
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!

private:
	void animate() {
		animation_timer += 16;
		// emulates old timer that fired once every 200 ms
		if (animation_timer < 200) {
			return;
		}
		animation_timer -= 200;

		for (int j = 0; j <= world.spritesInUse; ++j) {
			auto & s = world.Sprite[j];
			if (s.proc) {
				s.proc->animate(200);
			}
		}
	}

    long anyKey(int zed) {
        // Returns true if the player at the given index is pressing any key.
        return ((world.player_data[zed].RightKEY || world.player_data[zed].LeftKEY
                 || world.player_data[zed].upKey || world.player_data[zed].DownKEY
                 || world.player_data[zed].AttackKey) ? 1 : 0);
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

        //Rem-KIND DEFINITIONS
        //1 is player
        //2 is enemy
        //3 is fireball
        //4 is goomba thing
        //5 is unmoveable
        //6 is trampoline

        //to do
        //7 is a enemy killable by jumping on it
        //8 is bullet by the enemy (can pass through 5//s)

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
                //if (world.Sprite[k].z > (world.Sprite[j].length - (world.Sprite[j].length * 0.75))) then GoTo britneyDog2
            //2017: this label was unreferneced:
            // poddle2:
                //HolderJ = hitdetection(j, k, True)
                //if (HolderJ = 2 || HolderJ = 6 || HolderJ = 7) then
                //world.Sprite[j].x = world.Sprite[j].x - world.Sprite[j].mph
                //End If
                //if (HolderJ = 3 || HolderJ = 8 || HolderJ = 9) then
                //world.Sprite[j].x = world.Sprite[j].x + world.Sprite[j].mph
                //End If
                //if (HolderJ = 4 || HolderJ = 6 || HolderJ = 8) then
                //world.Sprite[j].y = world.Sprite[j].y - world.Sprite[j].mph
                //End If
                //if (HolderJ = 5 || HolderJ = 7 || HolderJ = 9) then
                //world.Sprite[j].y = world.Sprite[j].y + world.Sprite[j].mph
                //End If
                //if (hitdetection(k, j) = 1) then GoTo poddle2 //
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
        // 2017: unreferenced label:
        // poddle:
            //HolderJ = hitdetection(k, j, True)
            //if (HolderJ = 2 || HolderJ = 6 || HolderJ = 7) then
            //world.Sprite(k).x = world.Sprite(k).x - world.Sprite(k).mph
            //End If
            //if (HolderJ = 3 || HolderJ = 8 || HolderJ = 9) then
            //world.Sprite(k).x = world.Sprite(k).x + world.Sprite(k).mph
            //End If
            //if (HolderJ = 4 || HolderJ = 6 || HolderJ = 8) then
            //world.Sprite(k).y = world.Sprite(k).y - world.Sprite(k).mph
            //End If
            //if (HolderJ = 5 || HolderJ = 7 || HolderJ = 9) then
            //world.Sprite(k).y = world.Sprite(k).y + world.Sprite(k).mph
            //End If
            //if (hitdetection(k, j) = 1) then GoTo poddle
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
       //numberPlayers integer legend
       //1 Only player 1
       //2 Player 1 and 2
       //3 All three Players
       //4 Just player 2
       //5 Just player 3
       //6 Players 1 and 3
       //7 Players 2 and 3

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

    void cinemaM(int what) {
        //Everyone raise your hand who remembers cinemaM?
        //Yes this classic far too long subroutine has made a triumphant return from Lady Pousha Quest.
        if (what == 2) {
            world.Sprite[30].name = "script";
            world.Sprite[30].mode = "2";
            world.cinemaMax = 4;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1B1.wav", 2.24);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1B2.wav", 2.74);
            this->setCinema(2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1B3.wav", 4.24);
            this->setCinema(3, 5, 3, 7, 16, 15, 15, 15, 8, "TalkLv1B4.wav", 0.95);
        }
    }

    void createPlayer(int who) {
            int goatorg = 0;

        if (world.player_data[(who / 10)].playerName == "Thomas") {
            world.player_data[who / 10].weapon = "fireball";
            this->loadAnimation(who, "Thomas.ani");
            view.LoadTexture((who / 10) + 1, "Flip1.png", 254, 254);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++ goatorg) {
                this->loadAnimation(goatorg, "Fireball.ani");
            }
        }

        if (world.player_data[(who / 10)].playerName == "Nick") {
            world.player_data[who / 10].weapon = "fireball";
            this->loadAnimation(who, "nick.ani");
            view.LoadTexture((who / 10) + 1, "joel.png", 254, 258);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++goatorg) {
                this->loadAnimation(goatorg, "icespike.ani");
            }
        }

        if (world.player_data[(who / 10)].playerName == "Nicky") {
            world.player_data[who / 10].weapon = "bomb";
            this->loadAnimation(who, "nicky.ani");
            view.LoadTexture((who / 10) + 1, "LilNicky.png", 84, 148);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++goatorg) {
                this->loadAnimation(goatorg, "bomb.ani");
            }
        }

        for (goatorg = (who + 1); goatorg <= (who + 9); ++ goatorg) {
            world.Sprite[goatorg].name = "";
            world.Sprite[goatorg].zOrder = -90;
        }
    }

    void debugLevel() {
        LP3_ASSERT(false); // TODO
    }

    gsl::owner<GameProcess *> flipGame() {
        // I think this handles switching to different rooms or levels.
        int penguin;
        (void)penguin;  //2017- is this unused?

        if (world.exitS == "true" && boost::starts_with(world.screen, "Level")) {
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

		// MOVED THIS TO CONSTRUCTOR:
        //////************************************************************
        ////// LOADS A NEW LEVEL!------------------------------------
        //////************************************************************
        ////if (boost::starts_with(world.screen, "Level")
        ////    && world.currentScreen != world.screen) {
        ////    //as long as this is set to this crazy value, it won't load it again.
        ////    world.currentScreen = world.screen;

        ////    std::string crapple = world.screen.substr(5);
        ////    double boogeycrap = boost::lexical_cast<double>(crapple);

        ////    this->goToLevel(boogeycrap);
        ////}


        if (world.screen == "deadscreen") {
            world.screen = "title";
        }

        if (world.Sprite[0].name == "dead"
            && world.Sprite[10].name == "dead"
            && world.Sprite[20].name == "dead") {
			return create_gameover_screen(context);
        }

		return nullptr;
    }

    void findPlayers() {
        // TODO: is the madness of these next 3 lines really necessary?
        if (world.player_data[0].playerName == "") { world.player_data[0].playerName = "zgjkl"; }
        if (world.player_data[1].playerName == "") { world.player_data[1].playerName = "zgjkl"; }
        if (world.player_data[2].playerName == "") { world.player_data[2].playerName = "zgjkl"; }

		std::array<bool, 3> active_players = {
			world.Sprite[0].name == world.player_data[0].playerName,
			world.Sprite[10].name == world.player_data[1].playerName,
			world.Sprite[20].name == world.player_data[2].playerName,
		};
		world.numberPlayers
			= ActivePlayers::find_from_active_players(active_players);
    }

    int findQ(const std::string & who) {
        int opera = 0;
        int goatX = 0;


        goatX = 0;
        for (opera = 0; opera <= world.spritesInUse; ++ opera) {
            if (world.Sprite[opera].name == who) { goatX = goatX + 1; }
        }

        return goatX;
    }

    ////double getMiddleX(int who) {
    ////    return world.Sprite[who].x + (world.Sprite[who].wide / 2);
    ////}

    ////double getMiddleY(int who) {
    ////    return world.Sprite[who].y + (world.Sprite[who].high / 2);
    ////}

    void GoSub_level1a() {
        world.Sprite[31].frame = world.cinema[world.cinemaCounter].frame1;
        world.Sprite[32].frame = world.cinema[world.cinemaCounter].frame2;
        world.Sprite[33].frame = world.cinema[world.cinemaCounter].frame3;
        world.Sprite[34].frame = world.cinema[world.cinemaCounter].frame4;
        world.Sprite[31].color = view::qb_color(world.cinema[world.cinemaCounter].color1);
        world.Sprite[32].color = view::qb_color(world.cinema[world.cinemaCounter].color2);
        world.Sprite[33].color = view::qb_color(world.cinema[world.cinemaCounter].color3);
        world.Sprite[34].color = view::qb_color(world.cinema[world.cinemaCounter].color4);

        sound.PlayIsoWave(world.cinema[world.cinemaCounter].wavefile);
        world.Sprite[30].miscTime = world.clock + world.cinema[world.cinemaCounter].miscTime;
        world.cinemaCounter = world.cinemaCounter + 1;
    }

    void GoSub_level1b() {
        for (int i = 31; i <= 34; ++ i) {
            if (i == 30) { continue; } //2017: WAT?
            world.Sprite[i].visible = true;
            world.Sprite[i].wide = (271 - 137) * 2;
            world.Sprite[i].high = 81 * 2;
            world.Sprite[i].seekx = 0;
            world.Sprite[i].seeky = 1;
            world.Sprite[i].texture = 9;
        }
    }

    void GoSub_level1c() {
        world.Sprite[31].x = world.camera.CameraX - world.Sprite[31].seekx;
        world.Sprite[31].y = world.camera.CameraY + 20;
        world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[32].seekx;
        world.Sprite[32].y = world.camera.CameraY + 20;
        world.Sprite[33].x = world.camera.CameraX - world.Sprite[33].seekx;
        world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
        world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[33].seekx;
        world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;

        for (int i = 31; i <= 34; ++ i) {
            // If i = 34 Then GoTo kiddy2
            world.Sprite[i].seekx = world.Sprite[i].seekx + world.sFactor * 6;
            world.Sprite[i].seeky = world.Sprite[i].seeky + 1;
            if (world.Sprite[i].seeky == 3) {
                world.Sprite[i].seeky = 1;
            }
            world.Sprite[i].frame = world.Sprite[i].seeky;
        }

        if (world.Sprite[32].seekx > 268) {
            for (int i = 31; i <= 34; ++ i) {
                world.Sprite[i].visible = false;
            }

            world.Sprite[30].mode = "9";
        }
    }

    void GoSub_setUpLevel1() {
        for (int i = 31; i <= 34; ++ i) {
            if (i == 30) { continue; }
            world.Sprite[i].visible = true;
            world.Sprite[i].wide = (271 - 137) * 2;
            world.Sprite[i].high = 81 * 2;
            world.Sprite[i].seekx = 268;
            world.Sprite[i].seeky = 1;
            world.Sprite[i].texture = 9;
        }
    }

    void goToLevel(const double which) {
        world.Gravity = 0;

        if (which == 1.1 || which == 1) {
			destroyEverything(world, view, sound);
            this->MakeLevel("Level1Opening.ogg", "Level1.cap",
// TSNOW: This is such a hack, but honestly the graphic for the apartment
//        carpet- which already looked terrible - makes the eyes bleed
//        when the texture is manually repeated to make a 64x64 bitmap.
//        It's so much worse I'd rather only use it in Emscripten.
#ifdef LP3_COMPILE_TARGET_EMSCRIPTEN
                            "Lv1bg.png",
#else
                            "Lv1bg.bmp",
#endif
                            10, 10, "Level1Cinema.png", "Level1Cinema.ani",
                            true, true);
            world.cinemaMax = 2;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1A1.wav", 4.49);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1A2.wav", 1.87);
            world.Sprite[0].x = 50;
            world.Sprite[0].y = 220;
            world.Sprite[10].x = 50;
            world.Sprite[10].y = 220;
            world.Sprite[20].x = 50;
            world.Sprite[20].y = 220;
            world.camera.cameraStopX = 1010;
            world.camera.cameraStopY = 905 + 480;

            sound.LoadSound(16, "goomba.wav", "dying explosion");
            sound.LoadSound(17, "GoombaOuch.wav", "Goomba Ouch");
            sound.LoadSound(18, "KerboDie.wav", "Kerbose Die");
            sound.LoadSound(19, "KerbHurt.wav", "Kerbose Ouch");
            sound.LoadSound(20, "PutuloHurt.wav", "putulohurt");
            sound.LoadSound(21, "PutuloDie.wav", "putulodie");
        }

        if (which == 1.2) {
            this->MakeLevel("Level1.ogg", "level1b.cap", "Lv1bg2.png",
                            100, 100, "Level1BCinema.png", "Level1bCinema.ani",
                            true, true);
            world.cinemaMax = 3;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1C1.wav", 3.31);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1C2.wav", 2.3);
            this->setCinema(2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1C3.wav", 3.05);
            world.Sprite[0].x = 1122;
            world.Sprite[0].y = 1650;
            world.Sprite[10].x = 1122;
            world.Sprite[10].y = 1650;
            world.Sprite[20].x = 1122;
            world.Sprite[20].y = 1650;
            world.camera.cameraStopX = 1194;
            world.camera.cameraStopY = 1900;
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "BS Death.wav", "Paul Shrink");
            sound.LoadSound(20, "WhaWhee.wav", "Stick Awaken");
        }

        if (which == 1.3) {
            this->MakeLevel("Level1.ogg", "level1c.cap", "lv1bg3.png",
                            10, 10, "Level1BCinema.png", "Level1bCinema.ani",
                            false, false);
            world.cinemaMax = 3;
            world.cinemaCounter = 4;
            this->setCinema(0, 7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31);
            this->setCinema(1, 7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3);
            this->setCinema(2, 7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05);
            world.Sprite[0].x = 242;
            world.Sprite[0].y = 2000;
            world.Sprite[10].x = 42;
            world.Sprite[10].y = 300;
            world.Sprite[20].x = 42;
            world.Sprite[20].y = 300;
            world.camera.cameraStopX = 1244;
            world.camera.cameraStopY = 2273;
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (which == 1.4) {
            this->MakeLevel("Level1.ogg", "level1d.cap", "level1birdstreet.png",
                            98, 480, "Level1DCinema.png", "Level1bCinema.ani",
                            false, false);
            world.cinemaMax = 3;
            world.cinemaCounter = 4;
            this->setCinema(0, 7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31);
            this->setCinema(1, 7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3);
            this->setCinema(2, 7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05);
            world.Sprite[0].x = 42;
            world.Sprite[0].y = 300;
            world.Sprite[10].x = 42;
            world.Sprite[10].y = 300;
            world.Sprite[20].x = 42;
            world.Sprite[20].y = 300;
            world.camera.cameraStopX = 3000;
            world.camera.cameraStopY = 480;
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


        // 2017: unreferenced label:
        // screwthis:


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

        //if k = j) then GoTo screwthis3

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

        //TSNOW: This if statement just printed a line.
        //if (con3 == 1) { Print ""; } //Rem-Beep: //Label2.Caption = "From Top of " + Str$(j)

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
               //1 Only player 1
        //2 Player 1 and 2
        //3 All three Players
        //4 Just player 2
        //5 Just player 3
        //6 Players 1 and 3
        //7 Players 2 and 3
        int j = 0;
        world.Sprite[0].name = "dead";
        world.Sprite[10].name = "dead";
        world.Sprite[20].name = "dead";

		//2017: The old code set `gotFocus` to 5 if numberPlayers == 5. Every
		// other value matched the same magical gotFocus value to the same
		// numberPlayers value I'd seen elsewhere. I'm going to take a risk
		// and assume that was a bug in the old code.
		// if (world.numberPlayers == 5) { world.gotFocus = 5; }
		// world.gotFocus = world.numberPlayers.focus.gotFocus;

        //Dim j As Integer
        int k = 0;
        for (k = 0; k <= 2; ++ k) {
            if (world.player_data[k].playerName == "") { world.player_data[k].playerName = "redead"; }
        }
        //For j = 0 To (numberPlayers - 1)//
        //
        ////Sprite(j * 10).x = 50
        //Sprite(j * 10).y = 220
        //Sprite(j * 10).name = playerName(j)
        //For k = j * 10 + 1 To j * 10 + 9
        //Sprite(k).x = 60
        //Sprite(k).y = 70
        //Sprite(k).name = "fireball"
        //Next k
        //Next j

        //Rem****************88
        //After killing the players above by setting the sprites (bodies) to "dead",
        //it reincarnates them here by calling createPlayer
        //Rem*************************************8

        view.LoadTexture(0, "System.png", 336, 397);



        //Rem******************************************************8
        //          THIS PART LOADS UP WEAPONS
        //Rem******************************************************8

		for (int index = 0; index < 3; ++index) {
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
		}


        //Rem- THIS PART MAKES SPRITES DIFFERENT COLORS
        if (world.Sprite[0].name == world.Sprite[10].name) {
            world.Sprite[10].color = view::qb_color(10);
        }


        //Rem- This loads sounds into holders 0 to 14, meaning each player gets 5 sounds
        //Rem- Spring.wav is then put into slot 15
        //for (j = 0; j <= 2; ++ j) {

        //    if (world.player_data[j].playerName == "Thomas") {
        //        sound.LoadSound((j * 5), "fireball.wav", "fireball");
        //        sound.LoadSound((j * 5) + 1, "Death.wav", "DavidDeath");
        //        sound.LoadSound((j * 5) + 2, "hurt.wav", "DavidHurt");
        //    }

        //    if (world.player_data[j].playerName == "Nicky") {
        //        sound.LoadSound((j * 5), "NickyDeath.wav", "NickyDeath");
        //        sound.LoadSound((j * 5) + 1, "NickyHurt.wav", "NickyHurt");
        //        //2017: This file doesn't work, so don't play it
        //        //sound.LoadSound((j * 5) + 2, "SetBomb.wav ", "set bomb");
        //        sound.LoadSound((j * 5) + 3, "Bomb explode.wav", "bomb explode");
        //    }

        //    if (world.player_data[j].playerName == "Nick") {
        //        sound.LoadSound((j * 5), "nickdeath.wav", "nickdeath");
        //        sound.LoadSound((j * 5) + 1, "nickhurt.wav", "nickhurt");
        //        sound.LoadSound((j * 5) + 2, "iceshot.wav", "iceshot");
        //    }
        //}
        sound.LoadSound(15, "Spring.wav", "spring");
    }

    void killLimit(int jex) {
        // Rem- Kills unruly sprites who are out of bounds
        auto & s = world.Sprite[jex];
        if (s.x > world.camera.cameraStopX || s.x < -10) { kill(s); }

        if (s.y > world.camera.cameraStopX || s.y < -10) { kill(s); }
    }

    void loadAnimation(int who, const std::string & file) {
        auto & s = world.Sprite[who];
        view.load_animation_file(s.Aframe, file);
    }

    void level1() {
        //2017: Looks like this is never called
        LP3_ASSERT(false); // TODO
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
                ws.srcx = ws.srcx + (world.sFactor * 0.5);
                ws.srcx2 = ws.srcx2 + (world.sFactor * 0.5);
                ws.Aframe[1].x = ws.Aframe[1].x + 1;
                ws.Aframe[1].x2 = ws.Aframe[1].x2 + 1;
            }



            if (ws.name == "bullet") {     ////This is a strange type of bullet that in retrospect feels more like a bubble
                //if (ws.seekx <> -1) then

                this->killLimit(who);
				off_camera_kill(ws, Camera(world.camera));


                //if (ws.mode = "") then


                while(!(
                    (ws.seekx > world.camera.cameraStopX || ws.seekx < 0)
                        || (ws.seeky > world.camera.cameraStopY || ws.seeky < 0)
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
                    ws.x = ws.x + world.sFactor;
                    ws.seekx = ws.seekx + world.sFactor;
                    if (ws.seekx > 100) {
                        ws.mode = "left"; ws.seekx = 0; ws.dir = ""; }
                }
                if (ws.mode == "left") {
                    ws.reverse = true;
                    ws.x = ws.x - world.sFactor;
                    ws.seekx = ws.seekx + world.sFactor;
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
                if (k == 1) { ws.x = ws.x + world.sFactor; }
                if (k == 2) { ws.x = ws.x - world.sFactor; }
                k = random.next() * 2 + 1;
                if (k == 1) { ws.y = ws.y + world.sFactor; }
                if (k == 2) { ws.y = ws.y - world.sFactor; }
                k = random.next() * 20 + 1;
                if (k == 1) { if (ws.z == 0) {
                    ws.jumpStart = ws.z; ws.jumpTime = world.clock; } }
            }


            if (ws.name == "pigeonbomber") {
                ws.z = ws.z + world.sFactor;
                //ws.frame = ws.frame + 1
                //if (ws.frame > 2) then ws.frame = 1

                this->seeker(who);
                if (ws.x < 1) { ws.x = world.camera.cameraStopX; }

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
        //TSNOW: Unnecessary: string file = world.levelPath + fileArg;
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

    void MakeLevel(const std::string & lvlBgMusic, const std::string & levelFile,
        const std::string & levelBgFile, const int lvlBgWidth,
        const int lvlBgHeight,
        const std::string & CinemaBitMapFile,
        const std::string & CinemaAnimationFile,
        const bool stopMusic,
        const bool loadScreen) {

		destroyEverything(world, view, sound, 2);

        if (loadScreen == true) { this->NowLoading(); }

        world.camera.CameraWidth = 640; world.camera.CameraHeight = 480;

        if (stopMusic == true) { sound.PlayBgm(""); }


        this->loadLevel(levelFile); //"Level1b.cap"

        world.Gravity = 20;

        int j = 0;

        world.camera.CameraX = 0; world.camera.CameraY = 0;
        view.LoadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight); //"lv1bg2.png", 10, 10)
                                                                    //Call loadTexture(0, "smile.png", 255, 255)

                                                                    //Call loadTexture(4, "Level1a.png", 490, 209)
                                                                    //Call loadTexture(5, "lv1bgtw.png", 308, 341)
                                                                    //Call loadTexture(6, "goomba.png", 240, 240)
        view.LoadTexture(9, CinemaBitMapFile, 400, 400); //"lvl1bg.png",400,400


        this->initPlayers();

        //Call initPlayers
        //For j = 0 To 2
        //lives(j) = 3
        //continues(j) = 3
        //Next j

		// TODO: for j < 30; do player sprites, for j=30; do rest
		// JUKIE
		CharacterProcEnv env{ context, random, world.clock, camera };
		for (int h = 0; h < 3; ++h) {
			proc_manager.add_process(
				legacy_add_process(env, world, entity_manager, h*10,
					               world.Sprite[h*10], world.Sprite[h*10].name));
		}
        for (j = 30; j <= world.spritesInUse; ++j) {

			proc_manager.add_process(
				legacy_add_process(env, world, entity_manager, j,
					               world.Sprite[j], world.Sprite[j].name));
        }

        world.Sprite[33].name = "cinema"; world.Sprite[33].zOrder = -149;
        world.Sprite[32].name = "cinema"; world.Sprite[32].zOrder = -149;
        world.Sprite[31].name = "cinema"; world.Sprite[31].zOrder = -149;
        world.Sprite[34].name = "cinema"; world.Sprite[34].zOrder = -149;
        world.Sprite[30].zOrder = -149;
        this->loadAnimation(33, CinemaAnimationFile);
        this->loadAnimation(32, CinemaAnimationFile);
        this->loadAnimation(31, CinemaAnimationFile);
        this->loadAnimation(34, CinemaAnimationFile);
        world.Sprite[30].name = "StageWhat"; world.Sprite[30].mode = "";












        if (stopMusic == true) { sound.PlayBgm(lvlBgMusic); }


        world.exitS = "";
    }

    void NowLoading() {
        view.LoadTexture(-1, "NowLoading.png", 320, 240);
        world.camera.CameraWidth = 320;
        world.camera.CameraHeight = 240;

        world.camera.CameraWidth = 640;
        world.camera.CameraHeight = 480;
    }

    void script() {
        if (world.Sprite[30].mode != "3" && world.Sprite[30].mode != "2") {
            // move all the faces so that they line up
            world.Sprite[31].x = world.camera.CameraX; //  - world.Sprite[31].seekx
            world.Sprite[31].y = world.camera.CameraY + 20;
            world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268; // + world.Sprite[32].seekx
            world.Sprite[32].y = world.camera.CameraY + 20;
            world.Sprite[33].x = world.camera.CameraX; //  - world.Sprite[33].seekx
            world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
            world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268; //  + world.Sprite[33].seekx
            world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;
        }

        if (world.Sprite[30].mode == "6") {
            GoSub_level1c();
        }

        if (world.Sprite[30].mode == "5") {
            GoSub_level1b();
            world.Sprite[30].mode = "6";
        }
        if (world.Sprite[30].mode == "4" && world.Sprite[30].miscTime < world.clock) {
            if (world.cinemaCounter >= world.cinemaMax) {
                world.Sprite[30].mode = "5";
            } else {
                GoSub_level1a();
            }
        // world.Sprite[30].mode = "5"
        }
        if (world.Sprite[30].mode == "2") {
            if (world.cinemaCounter > world.cinemaMax) {
                world.Sprite[30].mode = "9";
                world.Sprite[31].visible = false;
                return;
            }
            GoSub_setUpLevel1();
            world.Sprite[30].mode = "3";
        }
        if (world.Sprite[30].mode == "3") {
            world.Sprite[31].x = world.camera.CameraX - world.Sprite[31].seekx;
            world.Sprite[31].y = world.camera.CameraY + 20;
            world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[32].seekx;
            world.Sprite[32].y = world.camera.CameraY + 20;
            world.Sprite[33].x = world.camera.CameraX - world.Sprite[33].seekx;
            world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
            world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[33].seekx;
            world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;

            if (world.Sprite[31].seekx < 0) {
                world.Sprite[30].mode = "4";
                world.Sprite[31].seekx = 0;
                world.Sprite[32].seekx = 0;
                world.Sprite[33].seekx = 0;
                world.Sprite[34].seekx = 0;
            }

            for (int i = 31; i <= 34; ++ i) {
                if (i == 96) { continue; }
                world.Sprite[i].seekx = world.Sprite[i].seekx - world.sFactor * 6;
                world.Sprite[i].frame = world.Sprite[i].frame + 1;
                if (world.Sprite[i].frame > 2) {
                    world.Sprite[i].frame = 1;
                }
                // Sprite(i).frame = Sprite(i).seeky
            }
        }
    }

    void seeker(int who) {
        auto & s = world.Sprite[who];
        if (s.x < s.seekx) { s.x = s.x + (s.mph * world.sFactor); }
        if (s.x > s.seekx) { s.x = s.x - (s.mph * world.sFactor); }
        if (s.y < s.seeky) { s.y = s.y + (s.mph * world.sFactor); }
        if (s.y > s.seeky) { s.y = s.y - (s.mph * world.sFactor); }
    }

    void setCinema(const int who, const int frame1, const int frame2,
                   const int frame3, const int frame4, const int color1,
                   const int color2, const int color3, const int color4,
                   const std::string & wavefile, const double miscTime) {
        auto & c = world.cinema[who];
        c.frame1 = frame1;
        c.frame2 = frame2;
        c.frame3 = frame3;
        c.frame4 = frame4;
        c.color1 = color1;
        c.color2 = color2;
        c.color3 = color3;
        c.color4 = color4;
        c.wavefile = wavefile;
        c.miscTime = miscTime;
    }

    void shoot(int who, const std::string & what, int wherex, int wherey) {
        int opera;

        for (opera = (who + 1); opera <= world.spritesInUse; ++ opera) {
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
