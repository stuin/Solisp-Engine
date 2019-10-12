/*
 * Created by Stuart Irwin on 9/23/2019.
 * Godot interface for Interpreter Game
 */

class GameData : public Reference {
	GDCLASS(GameData, Reference);

private:
	Solisp::Game game;
	Solisp::Card *initial = NULL;

public:
	GameData() {
	}

	void setup() {
		initial = game.setup(new Solisp::Builder());
	}

	CardData *next_card() {
		if(initial != NULL) {
			CardData *card = new CardData(initial);
			initial = initial->getNext();
			return card;
		}
		return NULL;
	}

	bool grab(int num, int from) {
		return game.grab(num, from);
	}

	bool test(int to) {
		return game.test(to);
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
		ClassDB::bind_method(D_METHOD("grab", "num", "from"), &GameData::grab);
		ClassDB::bind_method(D_METHOD("test", "to"), &GameData::test);
		ClassDB::bind_method(D_METHOD("place", "to"), &GameData::place);
		ClassDB::bind_method(D_METHOD("cancel"), &GameData::cancel);
	}
};