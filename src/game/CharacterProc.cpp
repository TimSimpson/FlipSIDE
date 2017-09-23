#include "CharacterProc.hpp"

#include "procs/PlayerProc.hpp"

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


void make_jump(CharacterSprite & sprite, double current_time) {
	if (sprite.z == 0) {
		sprite.multiJump = 0;
	}
	if (sprite.multiJump >= sprite.maxJump) {
		return;
	}
	sprite.multiJump = sprite.multiJump + 1;
	sprite.jumpStart = sprite.z;
	sprite.jumpTime = current_time;
}





int checkProx(World & world, const int who) {
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
	for (int penguin = 0; penguin <= 2; ++penguin) {
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

////void create_player(
////	PlayerData & player_data, CharacterSprite & sprite,
////	gsl::span<std::reference_wrapper<CharacterSprite>> & children,
////	CharacterProcEnv & env)
////{
////	LP3_ASSERT(children.size() == 9);
////
////	if (player_data.playerName == "Thomas") {
////		player_data.weapon = "fireball";
////		env.context.view.load_animation_file(sprite.Aframe, "Thomas.ani");
////		env.context.view.LoadTexture(player_data.sprite_index + 1, "Flip1.png", 254, 254);
////		sprite.texture = player_data.sprite_index + 1;
////		for (CharacterSprite & child : children) {
////			env.view.load_animation_file(child.Aframe, "Fireball.ani");
////		}
////	}
////
////	if (player_data.playerName == "Nick") {
////		player_data.weapon = "fireball";
////		env.view.load_animation_file(sprite.Aframe, "nick.ani");
////		env.view.LoadTexture(player_data.sprite_index + 1, "joel.png", 254, 258);
////		sprite.texture = player_data.sprite_index + 1;
////		for (CharacterSprite & child : children) {
////			env.view.load_animation_file(child.Aframe, "icespike.ani");
////		}
////	}
////
////	if (player_data.playerName == "Nicky") {
////		player_data.weapon = "bomb";
////		env.view.load_animation_file(sprite.Aframe, "nicky.ani");
////		env.view.LoadTexture(player_data.sprite_index + 1, "LilNicky.png", 84, 148);
////		sprite.texture = player_data.sprite_index + 1;
////		for (CharacterSprite & child : children) {
////			env.view.load_animation_file(child.Aframe, "bomb.ani");
////		}
////	}
////
////	for (CharacterSprite & child : children) {
////		child.name = "";
////		child.zOrder = -90;
////	}
////}

// For starters, all the old code is just shoved in here.
class LegacyProc : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & s;
	int j;
	World & world;

public:
     LegacyProc(CharacterProcEnv & _env, CharacterSprite & _s, int _j,
                World & world, const std::string & name)
     :  env(_env),
        s(_s),
		j(_j),
		world(world)
     {
        s.name = name;
        s.proc = this;
        initialize();
     }

	 ~LegacyProc() override = default;

	 void animate(std::int64_t ms) override {
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
		Random & random = env.random;

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
            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            spr.miscTime = current_time + 3;
            sound.PlaySound("stick die");
            spr.name = "Kerbose Death";
        }


        if (spr.name == "harharhar") {
            spr.flickerTime = current_time + 2;
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

        if (spr.name == "Kirbose" || spr.name == "Kerbose") {
            spr.mover = true;
            spr.name = "Kerbose";
            spr.wide = 21;
            spr.high = 19;
            spr.visible = true;
            spr.frame = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.length = 10;
            spr.deathType = "Kerbose Death";
            spr.invTime = 1;
            spr.hp = 3;
            spr.soundFile = "Kerbose Ouch";
            spr.jumpStrength = 25;
            const int trueorg = (int) (random.next() * 2.0 + 1);
            if (trueorg == 1) {
                view.load_animation_file(spr.Aframe, "Kerbose.ani");
            } else {
                view.load_animation_file(spr.Aframe, "Putulo.ani");
                spr.soundFile = "putulohurt";
                spr.deathType = "putuloDeath";
            }
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

        if (spr.name == "Kerbose Death") {
            spr.visible = true;
            //spr.name = "Dying Explosion"
            //spr.flickerTime = clock + 10
            //spr.seekx = spr.wide * 3
            //spr.mph = 2
            //spr.kind = Kind::neutral

            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            //srcx = 114: spr.srcy = 2
            //spr.srcx2 = 134: spr.srcy2 = 19
            spr.miscTime = current_time + 3;
            sound.PlaySound("Kerbose Die");
        }

        if (spr.name == "putuloDeath") {
            spr.visible = true;
            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            spr.miscTime = current_time + 3;
            sound.PlaySound("putulodie");
            spr.name = "Kerbose Death";
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
            spr.mover = true;
        }

        if (spr.name == "bullet") {
            spr.kind = Kind::enemy_bullet;
        }
    }

	bool update() override
		//Camera & camera,
		//PlayerData * player_data,
		//gsl::span<std::reference_wrapper<CharacterSprite>> & children) override
	//	gsl::span<std::reference_wrapper<CharacterSprite>> & children) override
	{
		auto & view = env.context.view;
		auto & sound = env.context.sound;
		auto & random = env.random;
		int k;

		//Rem-If s.time > clock Then GoTo gotDog
		//.time = clock + .speed
		//Rem---------------------------------------------------------------
		//'               THIS SECTION UPDATES THE DAVID SPRITE
		//Rem---------------------------------------------------------------



		  //- END OF DAVID SPRITE--------------------------------------------
		  //
		  //Rem--------------------------------------------------------------


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

		if (s.name == "Kerbose Death") {
			//.flickerTime = clock + 2
			//if (.color = QBColor(1) Then .color = normColor Else .color = QBColor(1)
			if (s.miscTime < world.clock) {
				s.name = "";
				s.visible = false;
				s.trueVisible = 2;
				s.flickerTime = world.clock + 1;
			}
		}

		if (s.name == "Kerbose") {
			k = (int)(random.next() * 2) + 1;
			if (k == 1) { s.x = s.x + world.sFactor; }
			if (k == 2) { s.x = s.x - world.sFactor; }
			k = (int)(random.next() * 2) + 1;
			if (k == 1) { s.y = s.y + world.sFactor; }
			if (k == 2) { s.y = s.y - world.sFactor; }
			k = (int)(random.next() * 20) + 1;
			if (k == 1) {
				if (s.z == 0) {
					s.jumpStart = s.z;
					s.jumpTime = world.clock;
				}
			}
		}



		if (s.name == "falling") {
			//.flickerTime = clock + 1
			s.z = s.z - world.sFactor;
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
				s.x = s.x - (s.mph * 0.5 * world.sFactor);
				s.high = s.high + s.mph;
				s.y = s.y - (s.mph * 0.5 * world.sFactor);
			}
			if (s.wide >= s.seekx) {
				s.high = s.high - s.mph;
				s.y = s.y + (s.mph * 0.5 * world.sFactor);
				if (s.high < 1) {
					s.name = "deceased";
					s.visible = false;
				}
			}
		}





		if (s.name == "Selecter") {
			if (world.clock > s.miscTime) {
				s.miscTime = world.clock + 0.1;  //this way, the person won't scream through the selection box because it moves at 40 FPS!
												 //For k = 0 To 2
				k = j;
				if (s.visible == false) {
					if (anyKey(world, k) == 1) {
						s.visible = true;
						s.frame = k + 1;
					}
				}

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

		if (s.name == "StageWhat") {
			//


		}

		//playWave "conten.wav"



		if (s.name == "exit") {
			//TSNOW: This looks to step up by 10 so should only iterate
			//       the loop once, but that's what the old code did.
			for (int penguin = 0; penguin <= 2; penguin += 10) {
				if (hit_detection(s, world.Sprite[penguin]) != 0
					&& world.Sprite[penguin].name
					== world.player_data[(penguin / 10)].playerName) {
					world.exitS = "true";
				}
			}
		}

		if (s.name == "tdigger") {
			if (s.mode == "") {
				unstretch(world.Sprite[j]);
				//TSNOW: Another funky step 10 loop.
				for (int penguin = 0; penguin <= 2; penguin += 10) {
					if (hit_detection(s, world.Sprite[penguin], true) == 5
						&& world.Sprite[penguin].name
						== world.player_data[(penguin / 10)].playerName) {
						s.mode = "runner";
						s.seekx = world.camera.cameraStopX;
						//.mhp = 10
						s.kind = Kind::enemy;
						s.deathType = "expand";
						s.time = 1;
						//.hp = 1
						s.reverse = true;
					}
				}
			}

			if (s.target != 0 || s.target == -1) {
				s.target = checkProx(world, j);
				s.seekx = s.getMiddleX();
				s.seeky = s.getMiddleY();
			}

			if (s.mode == "runner") {
				if (getProx(s, world.Sprite[s.target]) > 50) {
					s.seekx = world.Sprite[s.target].getMiddleX();
				}
				if (getProx(s, world.Sprite[s.target]) < 50) {
					s.mph = s.mph - 1;
					if (s.mph < -90) {
						s.seekx = world.Sprite[s.target].getMiddleX();
						s.mph = 10;
					}
				}
				s.seeky = world.Sprite[s.target].getMiddleY();
				if (getProx(s, world.Sprite[s.target]) < 100) {
					s.mph = (getProx(s, world.Sprite[s.target]) / 100);
				}
				else {
					s.mph = 10;
				}
				if (getProx(s, world.Sprite[s.target]) == 0) {
					s.seekx = world.Sprite[s.target].x;
					s.seeky = world.Sprite[s.target].y;
				}

				seek(s);

				//Stop
				//if (sgetMiddleX(.target) > getMiddleX(j)) then .x = .x + (sFactor * 3)
				//if (sgetMiddleX(.target) < getMiddleX(j)) then .x = .x - (sFactor * 3)

				//if (sgetMiddleY(.target) > getMiddleY(j)) then .y = .y + (sFactor * 1)
				//if (sgetMiddleY(.target) < getMiddleY(j)) then .y = .y - (sFactor * 1)
			}

		}


		if (s.name == "expand") {
			s.kind = Kind::neutral;
			//if (s.mode = "runner") then
			s.frame = 3;
			s.time += (0.01 * world.sFactor);
			s.wide = s.wide + (world.sFactor);
			s.x = s.x - (world.sFactor / 2);
			s.high = s.high + (world.sFactor);
			s.y = s.y - (world.sFactor / 2);
			if (s.time > 2) {
				s.name = "harharhar";
				initialize();
				// load_process(env, s, s.name); //: killS j
			}
		}

		if (s.name == "harharhar") {
			if (s.flickerTime < world.clock) {
				kill(s);
			}
		}


		if (s.name == "dead") {
			//Stop
			if ((j / 10) == 0) {
				s.x = world.camera.CameraX + 10;
			}
			if ((j / 10) == 1) {
				s.x = world.camera.CameraX + 250;
				s.color = view::qb_color(10);
			}
			if ((j / 10) == 2) {
				s.x = world.camera.CameraX + 450;
				s.color = view::qb_color(14);
			}
			s.y = world.camera.CameraY + 10;
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
//	// TODO: Pass the name in rather than relying on the name variable.
//	s.proc = load_process(s.name);
//	s.proc->initialize(env, s);
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
    for (int index = 0; index < procs.size(); ++ index) {
        if (!procs[index]->update()) {
			delete procs[index];
            procs.erase(procs.begin() + index);
			index -= 1;
        }
    }
}


gsl::owner<CharacterProc *> legacy_add_process(
	CharacterProcEnv & env, World & world, EntityManager & em,
	int j, CharacterSprite & s, const std::string & name)
{
	// TODO: in the future, use the names here, or something, but for
	//       now, use the indexes to figure out if it's a player
	if (j < 30) {
		return proc::create_player_proc(env, world.game_state, world.player_data[j / 10], em, name);
	} else {
		return new LegacyProc(env, s, j, world, name);
	}
}

}   }
