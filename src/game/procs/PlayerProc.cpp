#include "PlayerProc.hpp"
#include <limits>

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

namespace nnd3d { namespace game { namespace proc {

namespace {
	const glm::vec4 normColor{ 1.0f, 1.0f, 1.0f, 1.0f };
}

class FireballProc : public CharacterProc {
private:
	CharacterSprite & sprite;
	CharacterSprite & parent;
    Camera & camera;
	std::int64_t life_left;

public:
	FireballProc(CharacterProcEnv env, PlayerData & player_data,
		         CharacterSprite & _parent, CharacterSprite & _s)
	:	sprite(_s) ,
		parent(_parent),
        camera(env.camera),
		life_left(2 * 1000)
	{
		sprite.wide = 30 * (player_data.GradeUp + 1);
		sprite.high = 30 * (player_data.GradeUp + 1);
		sprite.length = 15;

		sprite.x = parent.x + (parent.wide / 2) - (sprite.wide / 2);
		sprite.y = parent.y + (parent.high / 2) - (sprite.high / 2);
		sprite.z = parent.z + (parent.length / 2);
		sprite.seekx = sprite.x;
		sprite.seeky = sprite.y;
		if (player_data.upKey || parent.dir == "u") {
			sprite.seeky =
				sprite.y - (env.camera.height() * 2);
			sprite.dir = "u";
		} else if (player_data.DownKEY || parent.dir == "d") {
			sprite.seeky =
				sprite.y + (2 * env.camera.height());
			sprite.dir = "d";
		}
		if (player_data.LeftKEY || parent.dir == "l") {
			sprite.seekx
				= sprite.x - (env.camera.width() * 2);
			sprite.dir = "l";
		} else if (player_data.RightKEY || parent.dir == "r") {
			sprite.seekx
				= sprite.x + (env.camera.width() * 2);
			sprite.dir = "r";
		}
        // TODO: This seemed like nonsense so I removed it, delete if it turns
        // out I was right:
		// if (player_data.RightKEY == true) {
		// 	sprite.seekx
		// 		= s.x + (2 * env.camera.width());
		// }
		// if (player_data.LeftKEY == true) {
		// 	sprite.seekx
		// 		= s.x - (env.camera.width() * 2);
		// }
		// if (player_data.upKey == true) {
		// 	sprite.seeky
		// 		= s.y - (env.camera.height() * 2);
		// }
		// if (player_data.DownKEY == true) {
		// 	sprite.seeky
		// 		= s.y + (env.camera.height() * 2);
		// }
		//Sprite(1).visible = true
		sprite.speed = 0; //0.00001
		sprite.name = "fireball";
		sprite.mph = 3;
		 //- (Sprite(0).length)		
		sprite.texture = parent.texture;
		sprite.visible = true;
		sprite.kind = Kind::fireball;
		//Sprite[k].soundFile = "fireball.wav"
		sprite.parent = player_data.index * 10;
		//LoadSound k, "fireball.wav"
		if (player_data.playerName == "Thomas") {
			env.context.sound.PlaySound("fireball");
		}
		if (player_data.playerName == "Nick") {
			env.context.sound.PlaySound("iceshot");
		}

		sprite.miscTime = env.current_time + 0.25;
	}

	void animate(std::int64_t ms) override {
		++ sprite.frame;
		if (sprite.frame > 3 || sprite.frame < 1) { sprite.frame = 1; }
	}

	void death_animation() override {
		throw std::logic_error("The method or operation is not implemented.");
	}

	void initialize() override {
		throw std::logic_error("The method or operation is not implemented.");
	}

	CharacterProc * spawn(CharacterSprite & sprite, const std::string & name) override {
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool update() override {
		// TODO: make the colors go crazy if the slicer mode is true.
		///*if (world.player_data[s.parent / 10].slicer == true) {
		//	if (s.color == view::qb_color(2)) {
		//		s.color = view::qb_color(15);
		//	}
		//	if (s.color == view::qb_color(7)) { s.color = view::qb_color(2); }
		//	if (s.color == view::qb_color(10)) { s.color = view::qb_color(7); }
		//	if (s.color == view::qb_color(14)) { s.color = view::qb_color(10); }
		//	if (s.color == view::qb_color(1)) { s.color = view::qb_color(14); }
		//	if (s.color == view::qb_color(4)) { s.color = view::qb_color(1); }
		//	if (s.color == view::qb_color(15)) { s.color = view::qb_color(4); }

		//	if ((int)(random.next() * 1) == 1) {
		//		s.jumpStrength = 75;
		//		s.maxJump = 2;
		//		make_jump(s, world.clock);
		//	}

		//*/}

		life_left -= ms_per_update;
		if (life_left <= 0) {
			kill(sprite);
			return false;
		}


		sprite.frame = sprite.frame + 1;
		if (sprite.dir == "r") {
			if (sprite.frame > 3) { sprite.frame = 1; }
		}
		if (sprite.dir == "l") {
			if (sprite.frame > 6) { sprite.frame = 4; }
		}
		if (sprite.dir == "u") {
			if (sprite.frame > 9) { sprite.frame = 7; }
		}
		if (sprite.dir == "d") {
			if (sprite.frame > 12) { sprite.frame = 10; }
		}
		//Rem----------------------
		sprite.flickerTime = std::numeric_limits<double>::max();
		if (sprite.x < sprite.seekx) { sprite.x = sprite.x + (sprite.mph * speed_factor); }
		if (sprite.x > sprite.seekx) { sprite.x = sprite.x - (sprite.mph * speed_factor); }
		if (sprite.y < sprite.seeky) { sprite.y = sprite.y + (sprite.mph * speed_factor); }
		if (sprite.y > sprite.seeky) { sprite.y = sprite.y - (sprite.mph * speed_factor); }
		//.wide = .wide + 1
		//.high = .high + 1
		//if (.x = .seekx And .y = .seeky Or (.x + .wide) < 1 Or .x > 640 Or (.y + .high) < 1 Or .y > 480 Then .visible = False: .name = "reserved"
		//if (.x >= .seekx And .x <= (.seekx + .mph) And .y >= .seeky And .y <= (.seeky + .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
		//if (.x <= .seekx And .x >= (.seekx - .mph) And .y <= .seeky And .y >= (.seeky - .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
		if (sprite.x > (camera.x() + camera.width())
			|| sprite.x < (camera.x() - camera.width())
			|| sprite.y >(camera.y() + camera.height())
			|| sprite.y < (camera.y() - camera.height())) {
			sprite.visible = false;
			sprite.flickerTime = 0;
			sprite.name = "reserved";
			sprite.trueVisible = 2;
		}

		off_camera_kill(sprite, camera);
        if (sprite.name == "") {    // if the function above just killed us
            return false;
        }
        return true;
	}

};

class PlayerProc : public CharacterProc {
private:
	CharacterProcEnv env;
    GameState & game_state;
    PlayerData & player_data;
    CharacterSprite & s;
    CharacterSprite & sprite;
    gsl::span<CharacterSprite> children;
	CharacterProcManager sub_processes;

public:
    PlayerProc(CharacterProcEnv _env,
               GameState & _game_state,
		       PlayerData & _player_data,
               EntityManager & e_manager,
               const std::string & name)
    :   env(_env),
        game_state(_game_state),
		player_data(_player_data),
        s(e_manager.grab_sprites(1)[0]),
        sprite(s),
        children(e_manager.grab_sprites(9)),
		sub_processes()
    {
        // See if doing `create_player` here would work or cause problems.
        s.name = name;
        s.proc = this;
		for (auto & c : children) { c.name = "reserved";  }
        initialize();
    }

	void animate(std::int64_t ms) {
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
		sub_processes.animate(ms);
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
	}

	// Find a free child sprite.
	CharacterSprite * find_free_sprite() {
		for (auto & child : children) {
			for (auto & child : children) {
				if (child.name == "reserved"
					|| child.name == "") {
					return &child;
				}
			}
		}
		return nullptr;
	}

	bool spawn_fireball() {
		auto * sprite = find_free_sprite();
		if (!sprite) {
			return false;
		}
		sub_processes.add_process(new FireballProc(env, player_data, s, *sprite));
		return true;
	}
	// Create a child process (think bullets)
	// TODO: take out of this base class once things settle down.
	CharacterProc * spawn(CharacterSprite & sprite, const std::string & name) override {
		return nullptr;
	}

	bool update() override {
		sub_processes.update();

        auto & view = env.context.view;
        auto & sound = env.context.sound;
        auto & random = env.random;

		if (s.name == "dead") {
			int f = 5;
		}
		else {
			int g = 2;
		}
        if (s.name == "Thomas" || s.name == "Nicky" || s.name == "Nick") {
			int penguin = player_data.index;

            if (player_data.upKey == true) {
                if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
                s.y = s.y - speed_factor;
                //s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
                s.speed = 0; //0.00001
                if (s.y < env.camera.y()) { s.y = env.camera.y(); }
            }
            if (player_data.DownKEY == true) {
                if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
                s.y = s.y + speed_factor;
                //s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
                s.speed = 0; //0.00001
                if (s.y > env.camera.y() + env.camera.height() - s.high) {
                    s.y = env.camera.y() + env.camera.height() - s.high;
                }
            }
            if (player_data.LeftKEY == true) {
                if (s.dir != "l" && player_data.upKey == false
                    && player_data.DownKEY == false) {
                    s.dir = "l";
                    s.frame = 14;
                }
                s.x = s.x - speed_factor;
                //s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
                s.speed = 0;  //0.00001
                if (s.x < env.camera.x()) { s.x = env.camera.x(); }
            }
            if (player_data.RightKEY == true) {
                if (s.dir != "r" && player_data.upKey == false
                    && player_data.DownKEY == false) {
                    s.dir = "r";
                    s.frame = 2;
                }
                s.x = s.x + speed_factor;
                //s.Frame = s.Frame + 1: if s.Frame > 3 Then s.Frame = 1
                s.speed = 0;  //0s.00001

                if (s.x > env.camera.x() + env.camera.width() - s.wide) {
                    s.x = env.camera.x() + env.camera.width() - s.wide;
                }
            }


            if (s.z == 0) { s.multiJump = 0; }

            if (s.name == "Nicky" && player_data.JumpKey == true
                && s.multiJump < 3) {
                player_data.JumpKey = false;
                //If .z = 0 Then .multiJump = 0
                s.multiJump = s.multiJump + 1;
                s.jumpStart = s.z;
                s.jumpTime = env.current_time;
            }


            if (player_data.JumpKey == true && s.z == 0) {

                s.jumpStart = s.z;
                s.jumpTime = env.current_time;
            }

            //Check for a lack of movement
            if (player_data.weapon == "bomb") {
                if (player_data.AttackKey == true
                    && s.miscTime < env.current_time) {
                    for (auto & child : children) {
                        if (child.name == "reserved"
                            || child.name == "") {

                            child.speed = 0; //0.00001
                            child.name = "bomb";
                            child.x = s.x;
                            child.y = s.y;
                            child.z = s.z; //- (Sprite(0).length);
                            child.wide = 30 * (player_data.GradeUp + 1);
                            child.high = 30 * (player_data.GradeUp + 1);
                            child.jumpStart = s.jumpStart;
                            child.jumpStrength = s.jumpStrength;
                            child.jumpTime = s.jumpTime;
                            child.length = 15;
                            child.texture = sprite.texture;
                            child.visible = true;
                            child.frame = 1;
                            child.trueVisible = 1;
                            child.kind = Kind::neutral;
                            child.mode = "";
                            child.miscTime = env.current_time + 3;
							child.parent = player_data.index * 10;
                            //2017: This file doesn't work:
                            // sound.PlaySound("set bomb");
                            //LoadSound k, "fireball.wav"
                            //PlaySound "fireball"
                            s.miscTime = env.current_time + 0.25;

                            break;
                        }
                    }
                }
            } //Nicky Bomb
              //Thomas Fire
            if (player_data.weapon == "fireball"
                && player_data.ThreeWay == false) {
                if (player_data.AttackKey == true
					&& s.miscTime < env.current_time)
				{					
					if (spawn_fireball()) {
						s.miscTime = env.current_time + 0.25;
					}
				}
            } //if thomas if

			// For now remove the neato three way fireball which come to think
			// of it I never got working in the C++ port did I.
      //      if (player_data.weapon == "fireball"
      //          && player_data.ThreeWay == true) {
      //          if (player_data.AttackKey == true
      //              && s.miscTime < env.current_time) {
      //              for (int k = 0; k < 6; ++ k) {
      //                  auto & child = children[k];
      //                  if (child.name == "reserved"
      //                      || child.name == "")
						//{
      //                      auto & next_child = children[k + 1];
						//	auto & next_child_2 = children[k + 2];
						//	if (s.dir == "l") {
						//		child.seeky = s.y;
						//		child.seekx
						//			= s.x - (env.camera.width() * 2);
						//		child.dir = "l";
						//		next_child.seekx
						//			= s.x - (env.camera.width() * 2);
						//		next_child.seeky
						//			= s.y + (env.camera.height() * 2);
						//		next_child.dir = "l";
						//		next_child_2.seekx
						//			= s.x - (env.camera.width() * 2);
						//		next_child_2.seeky
						//			= s.y - (env.camera.height() * 2);
						//		next_child_2.dir = "l";
						//	}
						//	if (s.dir == "r") {
						//		child.seeky = s.y;
						//		child.seekx
						//			= s.x + (env.camera.width() * 2);
						//		child.dir = "r";
						//		next_child.seekx
						//			= s.x + (env.camera.width() * 2);
						//		next_child.seeky
						//			= s.y + (env.camera.height() * 2);
						//		next_child.dir = "r";
						//		next_child_2.seekx
						//			= s.x + (env.camera.width() * 2);
						//		next_child_2.seeky
						//			= s.y - (env.camera.height() * 2);
						//		next_child_2.dir = "r";
						//	}
						//	if (player_data.upKey == true || s.dir == "u") {
						//		child.seekx = s.x;
						//		child.seeky = s.y - (env.camera.height() * 2);
						//		child.dir = "u";
						//		next_child.seekx
						//			= s.x - (env.camera.width() * 2);
						//		next_child.seeky
						//			= s.y - (env.camera.height() * 2);
						//		next_child.dir = "u";
						//		next_child_2.seekx
						//			= s.x + (env.camera.width() * 2);
						//		next_child_2.seeky
						//			= s.y - (env.camera.height() * 2);
						//		next_child_2.dir = "u";
						//		if (player_data.LeftKEY == true) {
						//			next_child_2.seeky = s.y;
						//			next_child_2.seekx
						//				= s.x - (env.camera.width() * 2);
						//			next_child_2.dir = "l";
						//		}
						//		if (player_data.RightKEY == true) {
						//			next_child.seeky = s.y;
						//			next_child.seekx
						//				= s.x + (env.camera.width() * 2);
						//			next_child.dir = "r";
						//		}
						//	}
						//	if (player_data.DownKEY == true
						//		|| s.dir == "d") {
						//		child.seekx = s.x;
						//		child.seeky = s.y + (2 * env.camera.height());
						//		child.dir = "d";
						//		next_child.seekx = s.x - (env.camera.width() * 2);
						//		next_child.seeky = s.y + (env.camera.height() * 2);
						//		next_child.dir = "d";
						//		next_child_2.seekx = s.x + (env.camera.width() * 2);
						//		next_child_2.seeky = s.y + (env.camera.height() * 2);
						//		next_child_2.dir = "d";
						//		if (player_data.LeftKEY == true) {
						//			next_child_2.seeky = s.y;
						//			next_child_2.seekx
						//				= s.x - (env.camera.width() * 2);
						//			next_child_2.dir = "l";
						//		}
						//		if (player_data.RightKEY == true) {
						//			next_child.seeky = s.y;
						//			next_child.seekx
						//				= s.x + (env.camera.width() * 2);
						//			next_child.dir = "r";
						//		}
						//	}
						//	if (s.mode == "truck") {
						//		child.seeky
						//			= env.camera.y() - env.camera.height();
						//		child.seekx = s.x;
						//		child.dir = "u";
						//	}
      //                      auto setup_fireball = [&](CharacterSprite & child_sprite) {
      //                          child_sprite.speed = 0;  //0.00001
      //                          child_sprite.name = "fireball";
      //                          child_sprite.mph = 3;
      //                          child_sprite.x = s.x;
      //                          child_sprite.y = s.y;
      //                          child_sprite.z = s.z; //- (world.Sprite[0).lengh)
      //                          child_sprite.wide
      //                              = 30 * (player_data.GradeUp + 1);
      //                          child_sprite.high
      //                              = 30 * (player_data.GradeUp + 1);
      //                          child_sprite.length = 15;
      //                          child_sprite.texture
      //                              = s.texture;
      //                          child_sprite.visible = true;
      //                          child_sprite.kind = Kind::fireball;
      //                          child_sprite.frame = 1;
      //                          child_sprite.soundFile = "fireball.wav";
      //                          child_sprite.parent = player_data.index / 10;
						//	};
						//	//Sprite(1).visible = True
      //                      setup_fireball(child);
      //                      setup_fireball(next_child);
      //                      setup_fireball(next_child_2);
						//	//LoadSound k, "fireball.wav"
						//	sound.PlaySound("fireball");
						//	s.miscTime = env.current_time + 0.25;
      //                      break;
      //                  }
      //              }
      //          }
            //} //if thomas if


            if (player_data.upKey == false
                && player_data.DownKEY == false
                && player_data.LeftKEY == false
                && player_data.RightKEY == false) {
                if (s.dir == "r") { s.frame = 2; }
                if (s.dir == "l") { s.frame = 14; }
                if (s.dir == "u") { s.frame = 6; }
                if (s.dir == "d") { s.frame = 10; }
                s.speed = 0;
            }

        } //-End of David Sprite If

        if (s.name == "Death of David") {
            //if (.seekx = 0) then .seekx = .x: .seeky = .y
            //if (.color = QBColor(4)) then .color = QBColor(1) Else .color = QBColor(4)
            s.flickerTime = env.current_time + 1;
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
                    player_data.lives = player_data.lives - 1;
                    if (player_data.lives < 1) {
                        if (!game_state.game_over()) {
                            s.name = "continue";
                            s.texture = 0;
                            s.srcx = 3;
                            s.srcy = 345;
                            s.srcx2 = 96;
                            s.srcy2 = 361;
                            s.wide = 93; //16;
                            s.high = 16; //93;
                            s.frame = 0;
                            auto & child = children[0];
                            child.name = "continue number";
                            child.color = normColor;
                            child.frame = 11;
                            child.texture = 0;
                            child.miscTime = env.current_time + 2;
                            view.load_animation_file(child.Aframe, "continue.ani");

                            s.kind = Kind::neutral;
                            child.kind = Kind::neutral;
                            child.visible = true;
                            child.wide = 20;
                            child.high = 20;
                            s.y = 10;
                            child.y = s.y;
                            s.y = 10;
                            child.x = s.x + 100;
                        } //end continue if
                    } //end lives if
                    if (player_data.lives > 0) {
						create_player(
							env, player_data, s,
							children);
                        s.name = player_data.playerName;
                        s.kind = Kind::player;

                        s.name = player_data.playerName;
                        initialize();
                        s.flickerTime = env.current_time + 5;
                        //Sprite[j].x = .seekx: .seekx = 0
                        //Sprite[j].y = .seekx: .seekx = 0

                    }

                }
            }
        }

        if (s.name == "continue") {
            if (player_data.any_key()) {
                game_state.use_continue();
                player_data.lives = 2;
				create_player(env, player_data, s, children);

                s.name = player_data.playerName;
                s.kind = Kind::player;
                s.flickerTime = env.current_time + 5;
                s.name = player_data.playerName;
                initialize();
            }
            s.visible = true;
            if (player_data.index == 0) {
                s.x = env.camera.x() + 10;
            }
            if (player_data.index == 1) {
                s.x = env.camera.x() + 250;
                s.color = view::qb_color(10);
            }
            if (player_data.index == 2) {
                s.x = env.camera.x() + 450;
                s.color = view::qb_color(14);
            }
            s.y = env.camera.y() + 10;
        }

        if (s.name == "continue number") {
            s.trueVisible = 1;
            if (player_data.index == 0) {
                s.x = env.camera.x() + 10 + 93;
            }
            if (player_data.index == 1) {
                s.x = env.camera.x() + 250 + 93;
            }
            if (player_data.index == 2) {
                s.x = env.camera.x() + 450 + 93;
            }
            s.y = env.camera.y() + 10;

            if (s.miscTime < env.current_time) {
                s.miscTime = env.current_time + 2;
                s.frame = s.frame - 1;
                if (s.frame == 0) { s.frame = 13; }
                s.visible = true;
                if (game_state.game_over()) {
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
					// TODO: RESTORE THIS! I thnik it killed the parent?
                    //world.Sprite[j - 1].name = "dead";
                    //world.Sprite[j - 1].visible = false;
                    kill(s); //": .visible = False
                }
            }
        }

        return true;
	}
};

CharacterProc * create_player_proc(
    CharacterProcEnv env, GameState & game_state,
    PlayerData & player_data, EntityManager & e_manager,
	const std::string & name)
{
	return new PlayerProc(env, game_state, player_data, e_manager, name);
}

}	}	}
