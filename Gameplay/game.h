namespace Solisp {
	class Game;
	struct Hand;
}

#include "../Gameplay/builder.h"
#include "../Gameplay/move.h"

#include <vector>
#include <queue>
#include <stdexcept>

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

struct Solisp::Hand {
	unc from = 0;
	unc to = 0;
	unc tested = 0;
	unsigned int count = 0;
};

class Solisp::Game {
private:
	//Game definition
	unc STACKCOUNT = MAXSTACKCOUNT;
	Hand users[MAXSTACKCOUNT];
	bool started = false;

	//Current game state
	Stack stack[MAXSTACKCOUNT];
	Move *current = new Move(0, 0, 0, false, false, NULL);

	//Other values

	//Apply current moves to stack array
	void apply(Move *move, bool reverse);

	//Startup functions
	void shuffle();
	void deal();

public:
	unc players = 1;

	Game() {}
	~Game() {
		current->clear_forward();
		delete current;
	}

	//Setup functions
	Card *setup(Builder *builder);
	void update();

	//General interaction methods
	bool grab(unsigned int num, unc from, unc user);
	bool test(unc to, unc user);
	bool place(unc to, unc user);

	//Clear hand
	void cancel(unc user);

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
	Stack *get_stack(unc i) {
		if(i >= STACKCOUNT)
			return NULL;
		return &stack[i];
	}
};
