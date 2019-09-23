#include "../Interpreter/game.h"

#include "core/reference.h"

/*
 * Created by Stuart Irwin on 9/23/2019.
 * Godot interface for Interpreter Game
 */

class Game : public Reference {
	GDCLASS(Game, Reference);

private:
	Solisp::Game game;

public:
	bool grab(int num, char from) {
		return game.grab(num, from);
	}

	bool place(char to) {
		return game.place(to);
	}

	void cancel() {
		game.cancel();
	}

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("grab", "num", "from"), &Game::grab);
		ClassDB::bind_method(D_METHOD("place", "to"), &Game::place);
		ClassDB::bind_method(D_METHOD("cancel"), &Game::cancel);
	}
}