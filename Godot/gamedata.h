/*
 * Created by Stuart Irwin on 9/23/2019.
 * Godot interface for Gameplay Game
 */

class GameData : public Reference {
	GDCLASS(GameData, Reference);

private:
	Solisp::Game game;
	Solisp::Card *initial = NULL;
	int next_index = 1;

public:
	GameData() {
	}

	void setup(int file) {
		std::cout << "Starting game setup \n";
		initial = game.setup(new Solisp::Builder(rule_files[file]));
	}

	CardData *next_card() {
		if(initial != NULL) {
			CardData *card = new CardData(initial);
			initial = initial->get_next();
			return card;
		}
		game.update();
		return NULL;
	}

	SlotData *next_slot() {
		if(game.get_stack(next_index) != NULL) {
			next_index++;
			return new SlotData(game.get_stack(next_index - 1));
		}
		return NULL;
	}

	bool grab(int num, int from) {
		return game.grab(num, from);
	}

	bool test(int to) {
		try {
			//std::cout << "Testing... ";
			if(to > 0 && to < game.get_stack_count())
				return game.test(to);
		} catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
		return false;
	}

	bool place(int to) {
		return game.place(to);
	}

	void cancel() {
		game.cancel();
	}

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("setup"), &GameData::setup);
		ClassDB::bind_method(D_METHOD("next_card"), &GameData::next_card);
		ClassDB::bind_method(D_METHOD("next_slot"), &GameData::next_slot);
		ClassDB::bind_method(D_METHOD("grab", "num", "from"), &GameData::grab);
		ClassDB::bind_method(D_METHOD("test", "to"), &GameData::test);
		ClassDB::bind_method(D_METHOD("place", "to"), &GameData::place);
		ClassDB::bind_method(D_METHOD("cancel"), &GameData::cancel);
	}
};