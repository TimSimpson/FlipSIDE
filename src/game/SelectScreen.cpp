#include "SelectScreen.hpp"
#include "CharacterProc.hpp"
#include "LegacyGame.hpp"
#include "NowLoading.hpp"

namespace nnd3d { namespace game {

namespace {
    struct Frame {
        glm::ivec2 ul;
        glm::ivec2 dr;
    };

    Frame frames[] = {
        {{1, 1}, {106, 182}},
        {{106, 1}, {210, 182}},
        {{210, 1}, {316, 182}},
        {{1, 183}, {106, 364}},
        {{106, 183}, {210, 364}},
    };
    constexpr int frame_count = sizeof(frames) / sizeof(frames[0]);

    struct SelectBox {
        SelectBox(const int _index, view::Billboard & _sprite, Sound & _sound)
        :   character(-1),
            finished(false),
            index(_index),
            sound(_sound),
            sprite(_sprite)
        {
            sprite.size = { 105 * 2, (217-36) * 2 };
            sprite.set_visibility(view::Visibility::invisible);
        }

        int character;
        bool finished;
        const int index;
        Sound & sound;
        view::Billboard & sprite;

        void activate() {
            sprite.set_visibility(view::Visibility::normal);
            sprite.texture_index = 1;
            set_character(index);
        }

        bool activated() const {
            return character != -1;
        }

        boost::optional<std::string> get_player_name() const {
            if (!finished) {
                return boost::none;
            }
            switch(character) {
                case 0:
                    return std::string{"Thomas"};
                case 1:
                    return std::string{"Nick"};
                case 2:
                    return std::string{"Andrew"};
                case 3:
                    return std::string{"Phil"};
                case 4:
                    return std::string{"Nicky"};
                default:
                    LP3_ASSERT(false);
                    return std::string("error");
            }
        }

        void handle_input(nnd3d::input::Key key, bool value) {
            if (value) {    // ignore everything but key down events
                if (!activated()) {
                    activate();
                    return;
                }
                switch(key) {
                    case input::Key::up:
                        set_character(character - 1);
                        break;
                    case input::Key::down:
                        set_character(character + 1);
                        break;
                    case input::Key::attack:
                    case input::Key::jump:
                        select();
                        break;
                    default:
                        break;
                }
            }
        }

        void select() {
            if (character == 0) { sound.PlayWave("PickTom.wav"); }
            if (character == 1) { sound.PlayWave("PickNick.wav"); }
            if (character == 2) { sound.PlayWave("PickDrew.wav"); }
            if (character == 4) { sound.PlayWave("PickNicky.wav"); }
            finished = true;
        }

        void set_character(int _character) {
            if (_character >= frame_count) {
                _character = frame_count - 1;
            } else if (_character < 0) {
                _character = 0;
            }
            character = _character;
            sprite.tex_src_ul = frames[character].ul;
            sprite.tex_src_dr = frames[character].dr;
        }
    };
}


class SelectScreen : public GameProcess
{
private:
	GameContext context;
	struct ClearBillboards {
		ClearBillboards(view::View & view) {
			view.billboards().clear();
			view.billboards().resize(4);
		}
	} clear_billboards;
	view::Billboard & bg;
	std::vector<SelectBox> boxes;


public:
    SelectScreen(GameContext _context,
                 std::array<bool, 3> keys_pressed)
    :   context(_context),
		clear_billboards(context.view),
        bg(context.view.billboards()[0]),
        boxes()
    {
        context.view.load_texture(0, "PlayerS.png", glm::ivec2{320, 240});
        bg.set(0, 0, 640, 480, 0, 0, 320, 240);
        bg.texture_index = 0;

		context.view.load_texture(1, "PlayerS2.png", glm::ivec2{320, 400});
        for (int i = 0; i < 3; ++ i) {
            boxes.emplace_back(i, context.view.billboards()[i + 1], context.sound);
        }

        boxes[0].sprite.ul = { 2 * 2, 36 * 2 };
        boxes[1].sprite.ul = { 106 * 2, 36 * 2 };
        boxes[2].sprite.ul = { 212 * 2, 36 * 2 };

        for (int i = 0; i < 2; ++ i) {
            if (keys_pressed[i]) {
                boxes[i].activate();
            }
        }

		context.sound.silence_sfx();
		context.sound.PlayBgm("Player SelectWAV.wav");
		context.sound.PlayWave("Select Your Characters of Justice.wav");
    }

	~SelectScreen() {
		int f = 5;
	}

    void handle_input(const input::Event & event) override {
        switch(event.key) {
            case input::Key::up:
            case input::Key::down:
            case input::Key::left:
            case input::Key::right:
            case input::Key::attack:
            case input::Key::jump:
                boxes[event.player].handle_input(event.key, event.value);
                break;
            default:
                break;
        }
    }

    gsl::owner<GameProcess *> update() override {
        for (auto & box : boxes) {
            if (box.activated() && !box.finished) {
                return nullptr;
            }
        }
		return start_game();
    }

    gsl::owner<GameProcess *> start_game() {
        // Initialize world stuff.
        World world;
        
		std::array<bool, 3> active;
		for (int i = 0; i < 3; ++i) {
			active[i] = boxes[i].finished;
		}
		world.numberPlayers = ActivePlayers::find_from_active_players(active);

        std::array<boost::optional<std::string>, 3> players;

        for (auto & box : boxes) {
            players[box.index] = box.get_player_name();

            world.player_data[box.index].lives = 3;
            world.player_data[box.index].playerName
                = box.get_player_name().get_value_or("");
        }        
        world.screen = "Level1.1";
        return create_legacy_screen(context, std::move(world), players);
    }
};  // end of GameImpl class


gsl::owner<GameProcess *> create_select_screen(
    GameContext context, std::array<bool, 3> keys_pressed)
{
	return create_now_loading_screen(
		context,
		[keys_pressed](GameContext context_2) {
			return new SelectScreen(context_2, keys_pressed);
	});    
}

namespace {
	gsl::owner<GameProcess *> create_select_screen_2(GameContext context) {
		return create_now_loading_screen(
			context,
			[](GameContext context_2) {
			std::array<bool, 3> keys_pressed{};
			keys_pressed[0] = true;
				return new SelectScreen(context_2, keys_pressed);
		});
	}

	RegisterGameProcess reg(
		"select-char", "The character select screen", create_select_screen_2);
}

}   }  // end namespace
