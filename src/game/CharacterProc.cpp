#include "CharacterProc.hpp"

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

void centerSprite(CharacterSprite & s, Camera & camera) {
	// Aims the camera on the given sprite.
	s.x = (camera.x() + (camera.width() / 2)) - (s.wide / 2);
	s.y = (camera.y() + (camera.height() / 2)) - (s.high / 2);
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




void create_player(
	CharacterProcEnv env, PlayerData & player_data, CharacterSprite & sprite,
	gsl::span<CharacterSprite> & children)
{

	int goatorg = 0;

	if (player_data.playerName == "Thomas") {
		player_data.weapon = "fireball";
		env.context.view.load_animation_file(sprite.Aframe, "Thomas.ani");
		env.context.view.LoadTexture(player_data.index + 1, "Flip1.png", 254, 254);
		sprite.texture = (player_data.index / 10) + 1;
		for (CharacterSprite & child : children) {
			env.context.view.load_animation_file(child.Aframe, "Fireball.ani");
		}
	}

	if (player_data.playerName == "Nick") {
		player_data.weapon = "fireball";
		env.context.view.load_animation_file(sprite.Aframe, "nick.ani");
		env.context.view.LoadTexture(player_data.index + 1, "joel.png", 254, 258);
		sprite.texture = (player_data.index / 10) + 1;
		for (CharacterSprite & child : children) {
			env.context.view.load_animation_file(child.Aframe, "icespike.ani");
		}
	}

	if (player_data.playerName == "Nicky") {
		player_data.weapon = "bomb";
		env.context.view.load_animation_file(sprite.Aframe, "nicky.ani");
		env.context.view.LoadTexture(player_data.index + 1, "LilNicky.png", 84, 148);
		sprite.texture = (player_data.index / 10) + 1;
		for (CharacterSprite & child : children) {
			env.context.view.load_animation_file(child.Aframe, "bomb.ani");
		}
	}

	for (CharacterSprite & child : children) {
		child.name = "";
		child.zOrder = -90;
	}
	//Sprite(who).name = what//playerName(who / 10)
	//Sprite(who).frame = 1
	//Call initSprites(who)
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
		if (penguin == 0 &&
			(world.numberPlayers == 4 || world.numberPlayers == 5 || world.numberPlayers == 7)) {
			continue;
		}
		if (penguin == 1 &&
			(world.numberPlayers == 1 || world.numberPlayers == 5 || world.numberPlayers == 6)) {
			continue;
		}
		if (penguin == 2 &&
			(world.numberPlayers == 1 || world.numberPlayers == 2 || world.numberPlayers == 4)) {
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
public:
     LegacyProc() = default;
	 ~LegacyProc() override = default;

	 void animate(CharacterSprite & s, std::int64_t ms) override {
		 //TODO: animation_timer was set to fire every 200ms- basically do
		 //      something similar, move this to view, and call it outside of
		 //      the logic loop.
		 if ((s.name == "Thomas" || s.name == "Nick") && s.mode != "truck") {
			 if (s.dir != "") { s.frame = s.frame + 1; }
			 if (s.dir == "u") {
				 if (s.frame > 8) { s.frame = 5; }
			 }
			 if (s.dir == "d") {
				 if (s.frame > 12) { s.frame = 9; }
			 }
			 if (s.dir == "l") {
				 if (s.frame > 16) { s.frame = 13; }
			 }
			 if (s.dir == "r") {
				 if (s.frame > 4) { s.frame = 1; }
			 }
		 }

		 if (s.name == "Nicky" && s.mode != "truck") {
			 if (s.dir != "") { s.frame = s.frame + 1; }
			 if (s.dir == "u") {
				 if (s.frame > 6) { s.frame = 4; }
			 }
			 if (s.dir == "d") {
				 if (s.frame > 9) { s.frame = 7; }
			 }
			 if (s.dir == "l") {
				 if (s.frame > 12) { s.frame = 10; }
			 }
			 if (s.dir == "r") {
				 if (s.frame > 3) { s.frame = 1; }
			 }
		 }

		 if (s.name == "fireball") {
			 s.frame = s.frame + 1;
			 if (s.frame > 3 || s.frame < 1) { s.frame = 1; }
		 }

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


	void initialize(CharacterProcEnv env, CharacterSprite & spr) override {
		view::View & view = env.context.view;
		Sound & sound = env.context.sound;
		const double current_time = env.current_time;
		Random & random = env.random;

        //this makes all the sprites do their thing based on their name
        spr.mph = 1;
        if (spr.name == "Thomas") {
            spr.zOrder = -99;
            spr.soundFile = "DavidHurt";
            spr.wide = 40;
            spr.high = 50;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 75;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of David";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 1;
        }

        if (spr.name == "Nick") {
            spr.zOrder = -99;
            spr.soundFile = "nickhurt";
            spr.wide = 40;
            spr.high = 50;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 75;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of Nick";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 1;
        }


        if (spr.name == "Nicky") {
            spr.zOrder = -99;
            spr.soundFile = "NickyHurt";
            spr.wide = 26;
            spr.high = 30;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 50;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of Nicky";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 2;
        }



        if (spr.name == "Death of David") {
            //StopSound which
            //LoadSound which, "Deathspr.wav"
            sound.PlaySound("DavidDeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;
            spr.frame = 17;
            sound.PlayWave("Death.wav");
        }
        if (spr.name == "Death of Nicky") {
            spr.srcx = 1;
            spr.srcy = 46;
            spr.srcx2 = 14;
            spr.srcy2 = 60;
            spr.name = "Death of David";
            sound.PlaySound("NickyDeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;
        }
        if (spr.name == "Death of Nick") {
            spr.srcx = 1;
            spr.srcy = 46;
            spr.srcx2 = 14;
            spr.srcy2 = 60;
            spr.name = "Death of David";
            sound.PlaySound("nickdeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;

        }




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

	void update(
		CharacterProcEnv env,
		CharacterSprite & s,
		int j,
		World & world) override
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

		if (s.name == "Thomas" || s.name == "Nicky" || s.name == "Nick") {
			int penguin = 0;
			if (j == 0) { penguin = 0; }
			if (j == 10) { penguin = 1; }
			if (j == 20) { penguin = 2; }

			if (world.player_data[penguin].upKey == true) {
				if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
				s.y = s.y - world.sFactor;
				//s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
				s.speed = 0; //0.00001
				if (s.y < world.camera.CameraY) { s.y = world.camera.CameraY; }
			}
			if (world.player_data[penguin].DownKEY == true) {
				if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
				s.y = s.y + world.sFactor;
				//s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
				s.speed = 0; //0.00001
				if (s.y > world.camera.CameraY + world.camera.CameraHeight - s.high) {
					s.y = world.camera.CameraY + world.camera.CameraHeight - s.high;
				}
			}
			if (world.player_data[penguin].LeftKEY == true) {
				if (s.dir != "l" && world.player_data[penguin].upKey == false
					&& world.player_data[penguin].DownKEY == false) {
					s.dir = "l";
					s.frame = 14;
				}
				s.x = s.x - world.sFactor;
				//s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
				s.speed = 0;  //0.00001
				if (s.x < world.camera.CameraX) { s.x = world.camera.CameraX; }
			}
			if (world.player_data[penguin].RightKEY == true) {
				if (s.dir != "r" && world.player_data[penguin].upKey == false
					&& world.player_data[penguin].DownKEY == false) {
					s.dir = "r";
					s.frame = 2;
				}
				s.x = s.x + world.sFactor;
				//s.Frame = s.Frame + 1: if s.Frame > 3 Then s.Frame = 1
				s.speed = 0;  //0s.00001

				if (s.x > world.camera.CameraX + world.camera.CameraWidth - s.wide) {
					s.x = world.camera.CameraX + world.camera.CameraWidth - s.wide;
				}
			}


			if (s.z == 0) { s.multiJump = 0; }

			if (s.name == "Nicky" && world.player_data[penguin].JumpKey == true
				&& s.multiJump < 3) {
				world.player_data[penguin].JumpKey = false;
				//If .z = 0 Then .multiJump = 0
				s.multiJump = s.multiJump + 1;
				s.jumpStart = s.z;
				s.jumpTime = world.clock;
			}


			if (world.player_data[penguin].JumpKey == true && s.z == 0) {

				s.jumpStart = s.z;
				s.jumpTime = world.clock;
			}

			//Check for a lack of movement
			if (world.player_data[j / 10].weapon == "bomb") {
				if (world.player_data[penguin].AttackKey == true
					&& s.miscTime < world.clock) {
					int k;
					for (k = j + 1; k < j + 10; ++k) {
						if (world.Sprite[k].name == "reserved"
							|| world.Sprite[k].name == "") {
							break;
						}
						if (k == j + 9) {
							k = j + 10;
							break;
						}
					}
					if (k != j + 10) {
						world.Sprite[k].speed = 0; //0.00001
						world.Sprite[k].name = "bomb";
						world.Sprite[k].x = s.x;
						world.Sprite[k].y = s.y;
						world.Sprite[k].z = s.z; //- (Sprite(0).length);
						world.Sprite[k].wide = 30 * (world.player_data[j / 10].GradeUp + 1);
						world.Sprite[k].high = 30 * (world.player_data[j / 10].GradeUp + 1);
						world.Sprite[k].jumpStart = s.jumpStart;
						world.Sprite[k].jumpStrength = s.jumpStrength;
						world.Sprite[k].jumpTime = s.jumpTime;
						world.Sprite[k].length = 15;
						world.Sprite[k].texture = world.Sprite[j].texture;
						world.Sprite[k].visible = true;
						world.Sprite[k].frame = 1;
						world.Sprite[k].trueVisible = 1;
						world.Sprite[k].kind = Kind::neutral;
						world.Sprite[k].mode = "";
						world.Sprite[k].miscTime = world.clock + 3;
						world.Sprite[k].parent = j;
						//2017: This file doesn't work:
						// sound.PlaySound("set bomb");
						//LoadSound k, "fireball.wav"
						//PlaySound "fireball"
						s.miscTime = world.clock + 0.25;
					}
				}
			} //Nicky Bomb
			  //Thomas Fire
			if (world.player_data[j / 10].weapon == "fireball"
				&& world.player_data[j / 10].ThreeWay == false) {
				if (world.player_data[penguin].AttackKey == true
					&& s.miscTime < world.clock) {
					for (k = j + 1; k <= j + 9; ++k) {
						if (world.Sprite[k].name == "reserved"
							|| world.Sprite[k].name == "") {
							break;
						}
						if (k == j + 9) {
							k = j + 10;
							break;
						}
					}
					if (k != j + 10) {
						if (s.dir == "u") {
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].seeky =
								s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k].dir = "u";
						}
						if (s.dir == "d") {
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].seeky =
								s.y + (2 * world.camera.CameraHeight);
							world.Sprite[k].dir = "d";
						}
						if (s.dir == "l") {
							world.Sprite[k].seeky = s.y;
							world.Sprite[k].seekx
								= s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k].dir = "l";
						}
						if (s.dir == "r") {
							world.Sprite[k].seeky = s.y;
							world.Sprite[k].seekx
								= s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k].dir = "r";
						}
						if (world.player_data[penguin].RightKEY == true) {
							world.Sprite[k].seekx
								= s.x + (2 * world.camera.CameraWidth);
						}
						if (world.player_data[penguin].LeftKEY == true) {
							world.Sprite[k].seekx
								= s.x - (world.camera.CameraWidth * 2);
						}
						if (world.player_data[penguin].upKey == true) {
							world.Sprite[k].seeky
								= s.y - (world.camera.CameraHeight * 2);
						}
						if (world.player_data[penguin].DownKEY == true) {
							world.Sprite[k].seeky
								= s.y + (world.camera.CameraHeight * 2);
						}
						if (s.mode == "truck") {
							world.Sprite[k].seeky
								= world.camera.CameraY - world.camera.CameraHeight;
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].dir = "u";
						}
						//Sprite(1).visible = true
						world.Sprite[k].speed = 0; //0.00001
						world.Sprite[k].name = "fireball";
						world.Sprite[k].mph = 3;
						world.Sprite[k].x = s.x;
						world.Sprite[k].y = s.y;
						world.Sprite[k].z = s.z; //- (Sprite(0).length)
						world.Sprite[k].wide = 30 * (world.player_data[j / 10].GradeUp + 1);
						world.Sprite[k].high = 30 * (world.player_data[j / 10].GradeUp + 1);
						world.Sprite[k].length = 15;
						world.Sprite[k].texture = world.Sprite[j].texture;
						world.Sprite[k].visible = true;
						world.Sprite[k].kind = Kind::fireball;
						//Sprite[k].soundFile = "fireball.wav"
						world.Sprite[k].parent = j;
						//LoadSound k, "fireball.wav"
						if (world.player_data[world.Sprite[k].parent / 10].playerName == "Thomas") {
							sound.PlaySound("fireball");
						}
						if (world.player_data[world.Sprite[k].parent / 10].playerName == "Nick") {
							sound.PlaySound("iceshot");
						}

						s.miscTime = world.clock + 0.25;
					}
				}
			} //if thomas if

			if (world.player_data[j / 10].weapon == "fireball"
				&& world.player_data[j / 10].ThreeWay == true) {
				if (world.player_data[penguin].AttackKey == true
					&& s.miscTime < world.clock) {
					for (k = j + 1; k <= j + 6; ++k) {
						if (world.Sprite[k].name == "reserved"
							|| world.Sprite[k].name == "") {
							break;
						}
						if (k == j + 9) {
							k = j + 10;
							break;
						}
					}
					if (k != j + 10) {
						if (s.dir == "l") {
							world.Sprite[k].seeky = s.y;
							world.Sprite[k].seekx
								= s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k].dir = "l";
							world.Sprite[k + 1].seekx
								= s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k + 1].seeky
								= s.y + (world.camera.CameraHeight * 2);
							world.Sprite[k + 1].dir = "l";
							world.Sprite[k + 2].seekx
								= s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k + 2].seeky
								= s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k + 2].dir = "l";
						}
						if (s.dir == "r") {
							world.Sprite[k].seeky = s.y;
							world.Sprite[k].seekx
								= s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k].dir = "r";
							world.Sprite[k + 1].seekx
								= s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k + 1].seeky
								= s.y + (world.camera.CameraHeight * 2);
							world.Sprite[k + 1].dir = "r";
							world.Sprite[k + 2].seekx
								= s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k + 2].seeky
								= s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k + 2].dir = "r";
						}
						if (world.player_data[penguin].upKey == true || s.dir == "u") {
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].seeky = s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k].dir = "u";
							world.Sprite[k + 1].seekx
								= s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k + 1].seeky
								= s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k + 1].dir = "u";
							world.Sprite[k + 2].seekx
								= s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k + 2].seeky
								= s.y - (world.camera.CameraHeight * 2);
							world.Sprite[k + 2].dir = "u";
							if (world.player_data[penguin].LeftKEY == true) {
								world.Sprite[k + 2].seeky = s.y;
								world.Sprite[k + 2].seekx
									= s.x - (world.camera.CameraWidth * 2);
								world.Sprite[k + 2].dir = "l";
							}
							if (world.player_data[penguin].RightKEY == true) {
								world.Sprite[k + 1].seeky = s.y;
								world.Sprite[k + 1].seekx
									= s.x + (world.camera.CameraWidth * 2);
								world.Sprite[k + 1].dir = "r";
							}
						}
						if (world.player_data[penguin].DownKEY == true
							|| s.dir == "d") {
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].seeky = s.y + (2 * world.camera.CameraHeight);
							world.Sprite[k].dir = "d";
							world.Sprite[k + 1].seekx = s.x - (world.camera.CameraWidth * 2);
							world.Sprite[k + 1].seeky = s.y + (world.camera.CameraHeight * 2);
							world.Sprite[k + 1].dir = "d";
							world.Sprite[k + 2].seekx = s.x + (world.camera.CameraWidth * 2);
							world.Sprite[k + 2].seeky = s.y + (world.camera.CameraHeight * 2);
							world.Sprite[k + 2].dir = "d";
							if (world.player_data[penguin].LeftKEY == true) {
								world.Sprite[k + 2].seeky = s.y;
								world.Sprite[k + 2].seekx
									= s.x - (world.camera.CameraWidth * 2);
								world.Sprite[k + 2].dir = "l";
							}
							if (world.player_data[penguin].RightKEY == true) {
								world.Sprite[k + 1].seeky = s.y;
								world.Sprite[k + 1].seekx
									= s.x + (world.camera.CameraWidth * 2);
								world.Sprite[k + 1].dir = "r";
							}
						}
						if (s.mode == "truck") {
							world.Sprite[k].seeky
								= world.camera.CameraY - world.camera.CameraHeight;
							world.Sprite[k].seekx = s.x;
							world.Sprite[k].dir = "u";
						}
						//Sprite(1).visible = True
						for (int trueorg = k; trueorg <= k + 2; ++trueorg) {
							world.Sprite[trueorg].speed = 0;  //0.00001
							world.Sprite[trueorg].name = "fireball";
							world.Sprite[trueorg].mph = 3;
							world.Sprite[trueorg].x = s.x;
							world.Sprite[trueorg].y = s.y;
							world.Sprite[trueorg].z = s.z; //- (world.Sprite[0).lengh)
							world.Sprite[trueorg].wide
								= 30 * (world.player_data[j / 10].GradeUp + 1);
							world.Sprite[trueorg].high
								= 30 * (world.player_data[j / 10].GradeUp + 1);
							world.Sprite[trueorg].length = 15;
							world.Sprite[trueorg].texture
								= world.Sprite[j].texture;
							world.Sprite[trueorg].visible = true;
							world.Sprite[trueorg].kind = Kind::fireball;
							world.Sprite[trueorg].frame = 1;
							world.Sprite[trueorg].soundFile = "fireball.wav";
							world.Sprite[trueorg].parent = j;
						}
						//LoadSound k, "fireball.wav"
						sound.PlaySound("fireball");
						s.miscTime = world.clock + 0.25;
					}
				}
			} //if thomas if


			if (world.player_data[penguin].upKey == false
				&& world.player_data[penguin].DownKEY == false
				&& world.player_data[penguin].LeftKEY == false
				&& world.player_data[penguin].RightKEY == false) {
				if (s.dir == "r") { s.frame = 2; }
				if (s.dir == "l") { s.frame = 14; }
				if (s.dir == "u") { s.frame = 6; }
				if (s.dir == "d") { s.frame = 10; }
				s.speed = 0;
			}

		} //-End of David Sprite If

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



		if (s.name == "fireball") {

			if (world.player_data[s.parent / 10].slicer == true) {
				if (s.color == view::qb_color(2)) {
					s.color = view::qb_color(15);
				}
				if (s.color == view::qb_color(7)) { s.color = view::qb_color(2); }
				if (s.color == view::qb_color(10)) { s.color = view::qb_color(7); }
				if (s.color == view::qb_color(14)) { s.color = view::qb_color(10); }
				if (s.color == view::qb_color(1)) { s.color = view::qb_color(14); }
				if (s.color == view::qb_color(4)) { s.color = view::qb_color(1); }
				if (s.color == view::qb_color(15)) { s.color = view::qb_color(4); }

				if ((int)(random.next() * 1) == 1) {
					s.jumpStrength = 75;
					s.maxJump = 2;
					make_jump(s, world.clock);
				}

			}


			s.frame = s.frame + 1;
			if (s.dir == "r") {
				if (s.frame > 3) { s.frame = 1; }
			}
			if (s.dir == "l") {
				if (s.frame > 6) { s.frame = 4; }
			}
			if (s.dir == "u") {
				if (s.frame > 9) { s.frame = 7; }
			}
			if (s.dir == "d") {
				if (s.frame > 12) { s.frame = 10; }
			}
			//Rem----------------------
			s.flickerTime = world.clock + 1;
			if (s.x < s.seekx) { s.x = s.x + (s.mph * world.sFactor); }
			if (s.x > s.seekx) { s.x = s.x - (s.mph * world.sFactor); }
			if (s.y < s.seeky) { s.y = s.y + (s.mph * world.sFactor); }
			if (s.y > s.seeky) { s.y = s.y - (s.mph * world.sFactor); }
			//.wide = .wide + 1
			//.high = .high + 1
			//if (.x = .seekx And .y = .seeky Or (.x + .wide) < 1 Or .x > 640 Or (.y + .high) < 1 Or .y > 480 Then .visible = False: .name = "reserved"
			//if (.x >= .seekx And .x <= (.seekx + .mph) And .y >= .seeky And .y <= (.seeky + .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
			//if (.x <= .seekx And .x >= (.seekx - .mph) And .y <= .seeky And .y >= (.seeky - .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
			if (s.x > (world.camera.CameraX + world.camera.CameraWidth)
				|| s.x < (world.camera.CameraX - world.camera.CameraWidth)
				|| s.y >(world.camera.CameraY + world.camera.CameraHeight)
				|| s.y < (world.camera.CameraY - world.camera.CameraHeight)) {
				s.visible = false;
				s.flickerTime = 0;
				s.name = "reserved";
				s.trueVisible = 2;
			}

			off_camera_kill(s, Camera(world.camera));
		}

		if (s.name == "bomb") {
			s.frame = s.frame + 1;
			if (s.mode != "explode") {
				if (s.frame > 3) { s.frame = 2; }
			}
			if (s.mode == "explode") {
				if (s.frame > 5) { s.frame = 4; }
			}
			if (s.kind == Kind::fireball && s.miscTime < world.clock) {
				s.visible = false;
				s.flickerTime = 0;
				s.name = "reserved";
				s.trueVisible = 2;
				s.kind = Kind::neutral;
				goto fuddle;
			}
			if (s.kind == Kind::fireball) {
				goto fuddle;
			}
			if (s.kind == Kind::neutral && s.miscTime > world.clock) {
				goto fuddle;
			}
			s.kind = Kind::fireball;
			s.miscTime = world.clock + 1;
			s.mode = "explode";
			sound.PlaySound("bomb explode");
			s.flickerTime = world.clock + 1;
		fuddle:
			EMPTY_LABEL_HACK
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

		if (s.name == "Death of David") {
			//if (.seekx = 0) then .seekx = .x: .seeky = .y
			//if (.color = QBColor(4)) then .color = QBColor(1) Else .color = QBColor(4)
			s.flickerTime = world.clock + 1;
			if (s.wide < s.seekx) {
				s.wide = s.wide + s.mph * 3; //instead of * 3, it used to be * 0.5
				s.high = s.high + s.mph * 3;
			}
			if (s.wide >= s.seekx) {
				s.high = s.high - s.mph * 3;
				if (s.high < (-1 * s.high)) {
					//what to do if dead
					s.name = "deceased";
					s.visible = false;
					s.name = "dead";
					s.srcx = 2;
					s.srcy = 363;
					s.srcx2 = 96;
					s.srcy2 = 379;
					s.texture = 0;
					s.visible = true;
					world.player_data[j / 10].lives = world.player_data[j / 10].lives - 1;
					if (world.player_data[j / 10].lives < 1) {
						if (world.continues > 0) {
							s.name = "continue";
							s.texture = 0;
							s.srcx = 3;
							s.srcy = 345;
							s.srcx2 = 96;
							s.srcy2 = 361;
							s.wide = 93; //16;
							s.high = 16; //93;
							s.frame = 0;
							world.Sprite[j + 1].name = "continue number";
							world.Sprite[j + 1].color = normColor;
							world.Sprite[j + 1].frame = 11;
							world.Sprite[j + 1].texture = 0;
							world.Sprite[j + 1].miscTime = world.clock + 2;
							view.load_animation_file(world.Sprite[j + 1].Aframe, "continue.ani");

							s.kind = Kind::neutral;
							world.Sprite[j + 1].kind = Kind::neutral;
							world.Sprite[j + 1].visible = true;
							world.Sprite[j + 1].wide = 20;
							world.Sprite[j + 1].high = 20;
							s.y = 10;
							world.Sprite[j + 1].y = s.y;
							s.y = 10;
							world.Sprite[j + 1].x = s.x + 100;
						} //end continue if
					} //end lives if
					if (world.player_data[j / 10].lives > 0) {
						create_player(
							env, world.player_data[(j / 10)], world.Sprite[j],
							gsl::make_span(&(world.Sprite[j + 1]), 9));
						world.Sprite[j].name = world.player_data[j / 10].playerName;
						world.Sprite[j].kind = Kind::player;
						load_process(env, s, world.player_data[j / 10].playerName);
						world.Sprite[j].flickerTime = world.clock + 5;
						//Sprite[j].x = .seekx: .seekx = 0
						//Sprite[j].y = .seekx: .seekx = 0

					}

				}
			}
		}

		if (s.name == "continue") {
			if (anyKey(world, j / 10) == 1) {
				world.continues = world.continues - 1;
				world.player_data[j / 10].lives = 2;
				create_player(
					env, world.player_data[(j / 10)], world.Sprite[j],
					gsl::make_span(&(world.Sprite[j + 1]), 9));

				world.Sprite[j].name = world.player_data[j / 10].playerName;
				world.Sprite[j].kind = Kind::player;
				world.Sprite[j].flickerTime = world.clock + 5;
				load_process(env, s, world.player_data[j / 10].playerName);
			}
			s.visible = true;
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
		}

		if (s.name == "continue number") {
			s.trueVisible = 1;
			if (((j - 1) / 10) == 0) {
				s.x = world.camera.CameraX + 10 + 93;
			}
			if (((j - 1) / 10) == 1) {
				s.x = world.camera.CameraX + 250 + 93;
			}
			if (((j - 1) / 10) == 2) {
				s.x = world.camera.CameraX + 450 + 93;
			}
			s.y = world.camera.CameraY + 10;

			if (s.miscTime < world.clock) {
				s.miscTime = world.clock + 2;
				s.frame = s.frame - 1;
				if (s.frame == 0) { s.frame = 13; }
				s.visible = true;
				if (world.continues < 1) {
					s.frame = 12; //this is useful, say, if two people have the choice of the last continue and one gets it before someone else
				}
				if (s.frame == 10) { sound.PlayWave("ConTen.wav"); }
				if (s.frame == 9) { sound.PlayWave("ConNine.wav"); }
				if (s.frame == 8) { sound.PlayWave("ConEight.wav"); }
				if (s.frame == 7) { sound.PlayWave("ConSeven.wav"); }
				if (s.frame == 6) { sound.PlayWave("ConSix.wav"); }
				if (s.frame == 5) { sound.PlayWave("ConFive.wav"); }
				if (s.frame == 4) { sound.PlayWave("ConFour.wav"); }
				if (s.frame == 3) { sound.PlayWave("ConThree.wav"); }
				if (s.frame == 2) { sound.PlayWave("ConTwo.wav"); }
				if (s.frame == 1) { sound.PlayWave("ConOne.wav"); }
				if (s.frame == 13) { sound.PlayWave("ConZero.wav"); }
				if (s.frame == 12) {
					world.Sprite[j - 1].name = "dead";
					world.Sprite[j - 1].visible = false;
					kill(s); //": .visible = False
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
			if (s.mode == "") {
				//playWave "conten.wav"
				if (s.miscTime == 0) {
					s.miscTime = world.clock + 2;
					world.Sprite[31].zOrder = -100;
					//findOrder(world);
				}

				{
					//TSNOW: No idea why this is hardcoded.
					auto & s2 = world.Sprite[31];
					if (world.Sprite[j].miscTime > world.clock) {
						s2.x = world.camera.CameraX;
						s2.y = world.camera.CameraY;
						s2.wide = 640;
						s2.high = 480;
						s2.frame = 9;
						s2.texture = 9;
						s2.visible = true;
					}

					if (world.Sprite[j].miscTime < world.clock) {
						s2.x = s2.x + 4 * world.sFactor;
						s2.y = s2.y + 4 * world.sFactor;
						s2.wide = s2.wide - 8 * world.sFactor;
						s2.high = s2.high - 8 * world.sFactor;
						s2.zOrder = -100;
						if (s2.wide < 0) {
							world.Sprite[j].mode = "?";
							world.Sprite[30].miscTime = world.clock + 3;
						}
					}
				}  // End With
			}  //mode
			if (s.mode == "?") {
				//playWave "conten.wav"
				{
					auto & s2 = world.Sprite[31];
					unstretch(s2);
					s2.wide = s2.wide * 2;
					s2.high = s2.high * 2;

					//.wide = (275 - 188) * 2
					//.high = (376 - 353) * 2

					s2.frame = 10;
					s2.texture = 9;
					s2.visible = true;
					s2.zOrder = -100;
					centerSprite(world.Sprite[31], Camera(world.camera));
				}
				if (s.miscTime < world.clock) {
					s.mode = "2";
					s.name = "script";
					//findOrder(world);
				}
			}


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
				load_process(env, s, s.name); //: killS j
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
	}
};

CharacterProc * load_process(const std::string &) {
    static LegacyProc legacy_proc;
    // TODO: Return other things
    return &legacy_proc;
}


void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &) {
	// TODO: Pass the name in rather than relying on the name variable.
	s.proc = load_process(s.name);
	s.proc->initialize(env, s);
}

}   }
