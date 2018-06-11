#include "PlayerProc.hpp"
#include <limits>
#include <lp3/sims.hpp>

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

namespace nnd3d { namespace game { namespace proc {

namespace { // begin anonymous namespace

const glm::vec4 normColor{ 1.0f, 1.0f, 1.0f, 1.0f };



void create_player(
    CharacterProcEnv env, PlayerData & player_data, CharacterSprite & sprite,
    gsl::span<CharacterSprite> & children)
{

    if (player_data.playerName == "Thomas") {
        player_data.weapon = "fireball";
        env.context.view.load_animation_file(sprite.Aframe, "Thomas.ani");
        env.context.view.LoadTexture(player_data.index + 1, "Flip1.png", 254, 254);
        sprite.texture = player_data.index + 1;
        for (CharacterSprite & child : children) {
            env.context.view.load_animation_file(child.Aframe, "Fireball.ani");
        }
    }

    if (player_data.playerName == "Nick") {
        player_data.weapon = "fireball";
        env.context.view.load_animation_file(sprite.Aframe, "nick.ani");
        env.context.view.LoadTexture(player_data.index + 1, "joel.png", 254, 258);
        sprite.texture = player_data.index + 1;
        for (CharacterSprite & child : children) {
            env.context.view.load_animation_file(child.Aframe, "icespike.ani");
        }
    }

    if (player_data.playerName == "Nicky") {
        player_data.weapon = "bomb";
        env.context.view.load_animation_file(sprite.Aframe, "nicky.ani");
        env.context.view.LoadTexture(player_data.index + 1, "LilNicky.png", 84, 148);
        sprite.texture = player_data.index + 1;
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


class FireballProc : public CharacterProc {
private:
    CharacterSprite & sprite;
    CharacterSprite & parent;
    Camera & camera;
    std::int64_t life_left;

public:
    FireballProc(CharacterProcEnv env, PlayerData & player_data,
                 CharacterSprite & _parent, CharacterSprite & _s,
                 glm::vec2 direction)
    :   sprite(_s) ,
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

        if (direction.x == 0.0f && direction.y == 0.0f) {
            if (parent.dir == "u") {
                direction.y = -1.0f;
                sprite.dir = "u";
            } else if ( parent.dir == "d") {
                direction.y = 1.0f;
                sprite.dir = "d";
            }
            if (parent.dir == "l") {
                direction.x = -1.0f;
                sprite.dir = "l";
            } else if (parent.dir == "r") {
                direction.x = 1.0f;
                sprite.dir = "r";
            }
        }

        sprite.x = parent.x + (parent.wide / 2) - (sprite.wide / 2);
        sprite.y = parent.y + (parent.high / 2) - (sprite.high / 2);
        sprite.z = parent.z + (parent.length / 2);
        sprite.seekx = sprite.x + (direction.x * env.camera.width() * 2);
        sprite.seeky = sprite.y + (direction.y * env.camera.height() * 2);;

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

    void animate(std::int64_t) override {
        ++ sprite.frame;
        if (sprite.frame > 3 || sprite.frame < 1) { sprite.frame = 1; }
    }

    bool update() override {
        // TODO: make the colors go crazy if the slicer mode is true.
        ///*if (world.player_data[s.parent / 10].slicer == true) {
        //  if (s.color == view::qb_color(2)) {
        //      s.color = view::qb_color(15);
        //  }
        //  if (s.color == view::qb_color(7)) { s.color = view::qb_color(2); }
        //  if (s.color == view::qb_color(10)) { s.color = view::qb_color(7); }
        //  if (s.color == view::qb_color(14)) { s.color = view::qb_color(10); }
        //  if (s.color == view::qb_color(1)) { s.color = view::qb_color(14); }
        //  if (s.color == view::qb_color(4)) { s.color = view::qb_color(1); }
        //  if (s.color == view::qb_color(15)) { s.color = view::qb_color(4); }

        //  if ((int)(random.next() * 1) == 1) {
        //      s.jumpStrength = 75;
        //      s.maxJump = 2;
        //      make_jump(s, world.clock);
        //  }

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
        if (sprite.x < sprite.seekx) { sprite.x = sprite.x + (sprite.mph * fs_speed_factor); }
        if (sprite.x > sprite.seekx) { sprite.x = sprite.x - (sprite.mph * fs_speed_factor); }
        if (sprite.y < sprite.seeky) { sprite.y = sprite.y + (sprite.mph * fs_speed_factor); }
        if (sprite.y > sprite.seeky) { sprite.y = sprite.y - (sprite.mph * fs_speed_factor); }

        if (sprite.x > (camera.x() + camera.width())
            || sprite.x < (camera.x() - camera.width())
            || sprite.y >(camera.y() + camera.height())
            || sprite.y < (camera.y() - camera.height())) {
            sprite.visible = false;
            sprite.flickerTime = 0;
            sprite.name = "reserved";
            sprite.trueVisible = 2;
        }

        return !off_camera_kill(sprite, camera);
    }

};


class BombProc : public CharacterProc {
private:
    CharacterSprite & sprite;
    std::int64_t timer;
    lp3::sims::CoroutineState coro_state;
    Sound & sound;
public:
    BombProc(CharacterProcEnv env, PlayerData & player_data,
             CharacterSprite & parent, CharacterSprite & _s)
    :   sprite(_s),
        timer(0),
        coro_state(),
        sound(env.context.sound)
    {
        sprite.speed = 0; //0.00001
        sprite.name = "bomb";
        sprite.x = parent.x;
        sprite.y = parent.y;
        sprite.z = parent.z; //- (Sprite(0).length);
        sprite.wide = 30 * (player_data.GradeUp + 1);
        sprite.high = 30 * (player_data.GradeUp + 1);
        jump_along_with(sprite, parent);
        sprite.length = 15;
        sprite.texture = parent.texture;
        sprite.visible = true;
        sprite.frame = 2;
        sprite.trueVisible = 1;
        sprite.kind = Kind::neutral;
        sprite.mode = "";
        sprite.parent = player_data.index * 10;
        //2017: This file doesn't work:
        // sound.PlaySound("set bomb");
        //LoadSound k, "fireball.wav"
        //PlaySound "fireball"

        // sprite.miscTime = env.current_time + 0.25;
        timer = 3000;
    }

    void animate(std::int64_t) override {
        sprite.frame = sprite.frame + 1;
        if (sprite.mode != "explode") {
            if (sprite.frame > 3) { sprite.frame = 2; }
        }
        if (sprite.mode == "explode") {
            if (sprite.frame > 5) { sprite.frame = 4; }
        }
    }

    bool update() override {
        timer -= ms_per_update;
        LP3_COROUTINE_BEGIN(coro_state);
            // Wait for bomb to explode
            while (timer > 0) {
                LP3_YIELD(true);
            }
            sprite.kind = Kind::fireball;

            // One second
            sprite.mode = "explode";
            sound.PlaySound("bomb explode");;
            sprite.flickerTime = std::numeric_limits<double>::max();
            timer = 1000;
            while (timer > 0) {
                LP3_YIELD(true);
            }
            sprite.visible = false;
            sprite.flickerTime = 0;
            sprite.name = "reserved";
            sprite.trueVisible = 2;
            sprite.kind = Kind::neutral;
        LP3_COROUTINE_END()
        return false;
    }
};

struct KeyData {
    //Keyboard key states
    bool upKey;
    bool DownKEY;
    bool LeftKEY;
    bool RightKEY;
    bool SelectKey;
    bool AttackKey;
    bool AttackKeyRelease;
    bool CancelKey;
    bool JumpKey;

    KeyData()
    :   upKey(false),
        DownKEY(false),
        LeftKEY(false),
        RightKEY(false),
        SelectKey(false),
        AttackKey(false),
        AttackKeyRelease(false),
        CancelKey(false),
        JumpKey(false)
    {}

    bool any_key() const {
        return RightKEY || LeftKEY || upKey || DownKEY || AttackKey;
    }
};

class PlayerProc : public InputReceivingCharacterProc {
protected:
    CharacterProcEnv env;
    GameState & game_state;
    PlayerData & player_data;
    CharacterSprite & s;
    CharacterSprite & sprite;
    gsl::span<CharacterSprite> children;
    CharacterProcManager sub_processes;
    lp3::sims::CoroutineState coro_number_state;
    std::int64_t timer;

    enum class State {
        normal,
        dying,
        continue_counter,
        inert
    };

    State state;

    KeyData key_data;

    glm::vec2 direction;
public:
    PlayerProc(CharacterProcEnv _env,
               GameState & _game_state,
               PlayerData & _player_data,
               EntityManager & e_manager,
               const std::string & name,
        const glm::vec2 & loc)
    :   env(_env),
        game_state(_game_state),
        player_data(_player_data),
        s(e_manager.grab_sprites(1)[0]),
        sprite(s),
        children(e_manager.grab_sprites(9)),
        sub_processes(),
        coro_number_state(),
        timer(),
        state(State::normal),
        key_data(),
        direction{0.0f, 0.0f}
    {
        create_player(env, player_data, sprite, children);

        // See if doing `create_player` here would work or cause problem.
        s.name = name;
        s.proc = this;
        for (auto & c : children) { c.name = "reserved";  }
        s.x = loc.x;
        s.y = loc.y;
    }

    virtual void animate_normally(std::int64_t ms) = 0;

    void animate(std::int64_t ms) override {
        if (State::normal == state) {
            animate_normally(ms);
        }
        sub_processes.animate(ms);
    }

    void become_inert() {
        sub_processes.clear();
        for (auto & c: children) {
            kill(c);
        }
        kill(sprite);
        state = State::inert;
        sprite.name = "dead";
        player_data.active = false;
    }

    virtual void _death_animation() = 0;

    void death_animation() override {
        state = State::dying;
        // Have to set the name or the camera scroll code breaks... good lord.
        sprite.name = "dying-animation";
        _death_animation();
    }


    virtual void _initialize() = 0;

    void initialize() override {
        state = State::normal;

        //view::View & view = env.context.view;
        //Sound & sound = env.context.sound;
        //const double current_time = env.current_time;
        //Random & random = env.random;

        auto & spr = this->s;

        spr.mph = 1;

        for (CharacterSprite & child : children) {
            child.name = "";
            child.zOrder = -90;
            child.trueVisible = 0;
            child.visible = false;
            child.flickerTime = 0;
        }
        _initialize();

        sprite.trueVisible = 0;
        sprite.visible = true;
        sprite.name = player_data.playerName;
    }

    // Find a free child sprite.
    CharacterSprite * find_free_sprite() {
        for (auto & child : children) {
            if (child.name == "reserved"
                || child.name == "") {
                return &child;
            }
        }
        return nullptr;
    }

    void handle_input(const input::Event & event) override {
        if (event.player != this->player_data.index) {
            return;
        }
        switch(event.key) {
            case input::Key::up:
                if (event.value != 0.0f) {
                    direction.y = -1.0f;
                } else if (direction.y < 0.0f) {
                    direction.y = 0.0f;
                }
                this->key_data.upKey = event.value != 0.0f;
                break;
            case input::Key::down:
                if (event.value != 0.0f) {
                    direction.y = 1.0f;
                } else if (direction.y > 0.0f) {
                    direction.y = 0.0f;
                }
                this->key_data.DownKEY = event.value != 0.0f;
                break;
            case input::Key::left:
                if (event.value != 0.0f) {
                    direction.x = -1.0f;
                } else if (direction.x < 0.0f) {
                    direction.x = 0.0f;
                }
                this->key_data.LeftKEY = event.value != 0.0f;
                break;
            case input::Key::right:
                if (event.value != 0.0f) {
                    direction.x = 1.0f;
                } else if (direction.x > 0.0f) {
                    direction.x = 0.0f;
                }
                this->key_data.RightKEY = event.value != 0.0f;
                break;
            case input::Key::attack:
                this->key_data.AttackKey = event.value != 0.0f;
                break;
            case input::Key::jump:
                this->key_data.JumpKey = event.value != 0.0f;
                break;
             case input::Key::power_up:
                this->player_data.slicer = true;
                this->player_data.GradeUp = 2;
                this->sprite.wide = 25;
                this->sprite.high = 25;
                this->env.context.sound.PlayWave("SoupedUp.wav");
                break;
            default:
                break;
        }
    }

    // Create a child process (think bullets)
    // TODO: take out of this base class once things settle down.
    CharacterProc * spawn(CharacterSprite &, const std::string &) override {
        return nullptr;
    }

    virtual bool spawn_weapon() = 0;

    void update_normal() {
        if (key_data.upKey == true) {
            if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
            s.y = s.y - fs_speed_factor;
            //s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
            s.speed = 0; //0.00001
            if (s.y < env.camera.y()) { s.y = env.camera.y(); }
        }
        if (key_data.DownKEY == true) {
            if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
            s.y = s.y + fs_speed_factor;
            //s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
            s.speed = 0; //0.00001
            if (s.y > env.camera.y() + env.camera.height() - s.high) {
                s.y = env.camera.y() + env.camera.height() - s.high;
            }
        }
        if (key_data.LeftKEY == true) {
            if (s.dir != "l" && key_data.upKey == false
                && key_data.DownKEY == false) {
                s.dir = "l";
                s.frame = 14;
            }
            s.x = s.x - fs_speed_factor;
            //s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
            s.speed = 0;  //0.00001
            if (s.x < env.camera.x()) { s.x = env.camera.x(); }
        }
        if (key_data.RightKEY == true) {
            if (s.dir != "r" && key_data.upKey == false
                && key_data.DownKEY == false) {
                s.dir = "r";
                s.frame = 2;
            }
            s.x = s.x + fs_speed_factor;
            //s.Frame = s.Frame + 1: if s.Frame > 3 Then s.Frame = 1
            s.speed = 0;  //0s.00001

            if (s.x > env.camera.x() + env.camera.width() - s.wide) {
                s.x = env.camera.x() + env.camera.width() - s.wide;
            }
        }


        if (s.z == 0) { s.multiJump = 0; }

        if (s.name == "Nicky" && key_data.JumpKey == true
            && s.multiJump < 3) {
            key_data.JumpKey = false;
            //If .z = 0 Then .multiJump = 0
            make_jump(s);
        }


        if (key_data.JumpKey == true && s.z == 0) {
            make_jump(s);
        }

        if (key_data.AttackKey == true && s.miscTime < env.current_time) {
            if (spawn_weapon()) {
                s.miscTime = env.current_time + 0.25;
            }
        }


        if (key_data.upKey == false
            && key_data.DownKEY == false
            && key_data.LeftKEY == false
            && key_data.RightKEY == false) {
            if (s.dir == "r") { s.frame = 2; }
            if (s.dir == "l") { s.frame = 14; }
            if (s.dir == "u") { s.frame = 6; }
            if (s.dir == "d") { s.frame = 10; }
            s.speed = 0;
        }
    }

    void orient_continue_text() {
        auto & continue_text = children[1];
        if (player_data.index == 0) {
            continue_text.x = env.camera.x() + 10;
        }
        if (player_data.index == 1) {
            continue_text.x = env.camera.x() + 250;
            continue_text.color = view::qb_color(10);
        }
        if (player_data.index == 2) {
            continue_text.x = env.camera.x() + 450;
            continue_text.color = view::qb_color(14);
        }
        continue_text.y = env.camera.y() + 10;

        auto & number = children[2];
        number.y = continue_text.y;
        number.x = continue_text.x + 100;
    }
    void start_continue_countdown() {
        state = State::continue_counter;
        if (!game_state.game_over()) {
            sprite.visible = false;
            sprite.trueVisible = 0;
            sprite.kind = Kind::neutral;

            // Delete all subprocesses
            sub_processes.clear();
            for (auto & c : children) { kill(c); }

            auto & continue_text = children[1];
            continue_text.trueVisible = 0;
            continue_text.flickerTime = 0;
            continue_text.zOrder = -149;
            continue_text.name = "continue";
            continue_text.texture = 0;
            continue_text.srcx = 3;
            continue_text.srcy = 345;
            continue_text.srcx2 = 96;
            continue_text.srcy2 = 361;
            continue_text.wide = 93; //16;
            continue_text.high = 16; //93;
            continue_text.frame = 0;

            continue_text.kind = Kind::neutral;
            continue_text.y = 10;
            continue_text.y = 10;

            continue_text.visible = true;

            auto & number = children[2];
            number.trueVisible = 0;
            number.flickerTime = 0;
            number.name = "continue number";
            number.zOrder = -149;
            number.color = normColor;
            number.frame = 11;
            number.texture = 0;
            number.miscTime = env.current_time + 2;
            env.context.view.load_animation_file(number.Aframe, "continue.ani");

            number.kind = Kind::neutral;
            number.visible = true;
            number.wide = 20;
            number.high = 20;

            orient_continue_text();
            number.y = continue_text.y;
            number.x = continue_text.x + 100;
        } else {
            become_inert();
        }
    }

    void update_dying() {
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
                s.srcx = 2;
                s.srcy = 363;
                s.srcx2 = 96;
                s.srcy2 = 379;
                s.texture = 0;
                s.visible = true;
                player_data.lives = player_data.lives - 1;
                if (player_data.lives <= 0) {
                    start_continue_countdown();
                } else {
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

    void update_continue_counter() {
        if (key_data.any_key()) {
            game_state.use_continue();
            player_data.lives = 2;
            create_player(env, player_data, s, children);

            s.name = player_data.playerName;
            s.kind = Kind::player;
            s.flickerTime = env.current_time + 5;
            s.name = player_data.playerName;
            state = State::normal;
            initialize();
        } else {
            orient_continue_text();
            timer += ms_per_update;
            auto & sound = env.context.sound;
            auto & number = children[2];
            LP3_COROUTINE_BEGIN(coro_number_state);
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 10;
            sound.PlayWave("ConTen.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 9;
            sound.PlayWave("ConNine.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 8;
            sound.PlayWave("ConEight.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 7;
            sound.PlayWave("ConSeven.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 6;
            sound.PlayWave("ConSix.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 5;
            sound.PlayWave("ConFive.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 4;
            sound.PlayWave("ConFour.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 3;
            sound.PlayWave("ConThree.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 2;
            sound.PlayWave("ConTwo.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 1;
            sound.PlayWave("ConOne.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            number.frame = 13;
            sound.PlayWave("ConZero.wav"); timer = 0;
            while (timer < 2000) { LP3_YIELD(); }
            become_inert();
            LP3_COROUTINE_END();
        }
    }

    bool update() override {
        sub_processes.update();

        switch (state)
        {
        case State::normal:
            update_normal();
            break;
        case State::dying:
            update_dying();
            break;
        case State::continue_counter:
            update_continue_counter();
            break;
        case State::inert:
            break;
        default:
            LP3_ASSERT(false);
        }
        return true;
    }
};


class ThomasProc : public PlayerProc {
private:
    bool mover;

public:
    ThomasProc(CharacterProcEnv _env,
        GameState & _game_state,
        PlayerData & _player_data,
        EntityManager & e_manager,
        const glm::vec2 & loc)
    :   PlayerProc(_env, _game_state, _player_data, e_manager, "Thomas", loc),
        mover(false)
    {
        env.context.sound.LoadSound((player_data.index * 5), "fireball.wav", "fireball");
        env.context.sound.LoadSound((player_data.index * 5) + 1, "Death.wav", "DavidDeath");
        env.context.sound.LoadSound((player_data.index * 5) + 2, "hurt.wav", "DavidHurt");
        initialize();
    }

    void animate_normally(std::int64_t) override {
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

    void _death_animation() override {
        state = State::dying;

        env.context.sound.PlaySound("DavidDeath");
        sprite.seekx = sprite.wide * 10;
        sprite.mph = 2;
        sprite.kind = Kind::neutral;
        sprite.frame = 17;
        env.context.sound.PlayWave("Death.wav");
    }

    void _initialize() override {
        state = State::normal;
        //view::View & view = env.context.view;
        //Sound & sound = env.context.sound;
        //const double current_time = env.current_time;
        //Random & random = env.random;

        sprite.zOrder = -99;
        sprite.soundFile = "DavidHurt";
        sprite.wide = 40;
        sprite.high = 50;
        sprite.texture = player_data.index + 1;
        //sprite.texture = 1;
        sprite.visible = true;
        sprite.length = 20;
        sprite.z = 0;
        sprite.jumpStrength = 75;
        sprite.kind = Kind::player;
        sprite.frame = 5;
        sprite.dir = "u";
        sprite.invTime = 2;
        sprite.speed = 0;
        sprite.deathType = "Death of David";
        sprite.hp = 4;
        //sprite.mode = "truck";
        this->mover = true;
        sprite.maxJump = 1;
    }

    bool spawn_weapon() override {
        auto * free_sprite = find_free_sprite();
        if (!free_sprite) {
            return false;
        }
        sub_processes.add_process(
            new FireballProc(env, player_data, s, *free_sprite, direction));
        return true;
    }
};


class NickProc : public ThomasProc {
private:
    bool mover;
public:
    NickProc(CharacterProcEnv _env,
        GameState & _game_state,
        PlayerData & _player_data,
        EntityManager & e_manager,
        const glm::vec2 & loc)
    :   ThomasProc(_env, _game_state, _player_data, e_manager, loc),
        mover(false)
    {
        env.context.sound.LoadSound((player_data.index * 5), "nickdeath.wav", "nickdeath");
        env.context.sound.LoadSound((player_data.index * 5) + 1, "nickhurt.wav", "nickhurt");
        env.context.sound.LoadSound((player_data.index * 5) + 2, "iceshot.wav", "iceshot");

        initialize();
    }

    void _death_animation() override {
        state = State::dying;

        sprite.srcx = 1;
        sprite.srcy = 46;
        sprite.srcx2 = 14;
        sprite.srcy2 = 60;
        sprite.name = "Death of David";
        env.context.sound.PlaySound("nickdeath");
        sprite.seekx = sprite.wide * 10;
        sprite.mph = 2;
        sprite.kind = Kind::neutral;
    }

    void _initialize() override {
        state = State::normal;

        // view::View & view = env.context.view;
        // Sound & sound = env.context.sound;
        // const double current_time = env.current_time;
        // Random & random = env.random;

        sprite.zOrder = -99;
        sprite.soundFile = "nickhurt";
        sprite.wide = 40;
        sprite.high = 50;
        //sprite.texture = 1;
        sprite.visible = true;
        sprite.length = 20;
        sprite.z = 0;
        sprite.jumpStrength = 75;
        sprite.kind = Kind::player;
        sprite.frame = 5;
        sprite.dir = "u";
        sprite.invTime = 2;
        sprite.speed = 0;
        sprite.deathType = "Death of Nick";
        sprite.hp = 4;
        //sprite.mode = "truck";
        this->mover = true;
        sprite.maxJump = 1;
    }
};

class NickyProc : public PlayerProc {
private:
    bool mover;
public:
    NickyProc(CharacterProcEnv _env,
        GameState & _game_state,
        PlayerData & _player_data,
        EntityManager & e_manager,
        const glm::vec2 & loc)
    :   PlayerProc(_env, _game_state, _player_data, e_manager, "Nicky", loc),
        mover(false)
    {
        env.context.sound.LoadSound((player_data.index * 5), "NickyDeath.wav", "NickyDeath");
        env.context.sound.LoadSound((player_data.index * 5) + 1, "NickyHurt.wav", "NickyHurt");
        //2017: This file doesn't work, so don't play it
        //env.context.sound.LoadSound((player_data.index * 5) + 2, "SetBomb.wav ", "set bomb");
        env.context.sound.LoadSound((player_data.index * 5) + 3, "Bomb explode.wav", "bomb explode");

        initialize();
    }

    void animate_normally(std::int64_t) override {
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

    void _death_animation() override {
        state = State::dying;

        sprite.srcx = 1;
        sprite.srcy = 46;
        sprite.srcx2 = 14;
        sprite.srcy2 = 60;
        sprite.name = "Death of David";
        env.context.sound.PlaySound("NickyDeath");
        sprite.seekx = sprite.wide * 10;
        sprite.mph = 2;
        sprite.kind = Kind::neutral;
    }

    void _initialize() override {
        state = State::normal;

        // view::View & view = env.context.view;
        // Sound & sound = env.context.sound;
        // const double current_time = env.current_time;
        // Random & random = env.random;


        sprite.zOrder = -99;
        sprite.soundFile = "NickyHurt";
        sprite.wide = 26;
        sprite.high = 30;
        //sprite.texture = 1;
        sprite.visible = true;
        sprite.length = 20;
        sprite.z = 0;
        sprite.jumpStrength = 50;
        sprite.kind = Kind::player;
        sprite.frame = 5;
        sprite.dir = "u";
        sprite.invTime = 2;
        sprite.speed = 0;
        sprite.deathType = "Death of Nicky";
        sprite.hp = 4;
        //sprite.mode = "truck";
        this->mover = true;
        sprite.maxJump = 3;
    }

    bool spawn_weapon() override {
        auto * free_sprite = find_free_sprite();
        if (!free_sprite) {
            return false;
        }
        sub_processes.add_process(new BombProc(env, player_data, s, *free_sprite));
        return true;
    }
};


}   // end anonymous namespace

InputReceivingCharacterProc * create_player_proc(
    CharacterProcEnv env, GameState & game_state,
    PlayerData & player_data, EntityManager & e_manager, const glm::vec2 & loc)
{
    if (player_data.playerName == "Thomas") {
        return new ThomasProc(env, game_state, player_data, e_manager, loc);
    } else if (player_data.playerName == "Nick") {
        return new NickProc(env, game_state, player_data, e_manager, loc);
    } else if (player_data.playerName == "Nicky") {
        return new NickyProc(env, game_state, player_data, e_manager, loc);
    } else {
        return nullptr;
    }
}

}   }   }
