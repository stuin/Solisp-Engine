namespace Solisp {
	class Game;
}

#include <vector>
#include <queue>
#include <stdexcept>

#include "../Lisp/game_env.h"

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

class Solisp::Game {
private:
	//Game definition
	char STACKCOUNT = MAXSTACKCOUNT;
	GameEnviroment env;

	//Current game state
	Stack stack[MAXSTACKCOUNT];
	Move *current = new Move(0, 0, 0, false, false, NULL);

	//Current move
	int from = -1;
	int to = -1;
	int count = 0;
	int tested = -1;

	//Other values
	bool started = false;

	//Apply current moves to stack array
	void apply(Move *move, bool reverse);

	//Startup functions
	void shuffle();
	void deal();

public:

	Game() {}
	~Game() {
		current->clear_forward();
		delete current;
	}

	//Setup functions
	Card *setup(Builder *builder);
	void update();

	//General interaction methods
	bool grab(int num, int from, int user=0);
	bool test(int to, int user=0);
	bool place(int to, int user=0);

	//Clear hand
	void cancel();

	//History management
	void undo();
	void redo();
	Move *get_current() {
		return current;
	}

	//Stack access
	int get_stack_count() {
		return STACKCOUNT;
	}
	Stack *get_stack(int i) {
		if(i >= STACKCOUNT)
			return NULL;
		return &stack[i];
	}
};
