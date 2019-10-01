/*
 * Created by Stuart Irwin on 9/23/2019.
 * Godot interface for Interpreter Game
 */

class GameData : public Reference {
	GDCLASS(GameData, Reference);

private:
	Solisp::Game game;

public:
	GameData() {}

	bool grab(int num, int from) {
		return game.grab(num, from);
	}

	bool place(int to) {
		return game.place(to);
	}

	void cancel() {
		game.cancel();
	}

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("grab", "num", "from"), &GameData::grab);
		ClassDB::bind_method(D_METHOD("place", "to"), &GameData::place);
		ClassDB::bind_method(D_METHOD("cancel"), &GameData::cancel);
	}
};