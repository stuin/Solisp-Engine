namespace Solisp {
	class Game;
	struct Hand;
}

#include "../Gameplay/builder.h"
#include "../Gameplay/move.h"

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
	unc STACKCOUNT = 0;
	Hand *users = NULL;
	bool started = false;

	//Current game state
	Stack stack[64];
	Move *current = NULL;
	unsigned int cardsLeft = 0;

	//Apply current moves to stack array
	void apply(Move *move, bool reverse);

	//Startup functions
	void shuffle();
	void deal();

public:
	unc players = 2;

	Game() {}
	~Game() {
		clear();
	}

	//Setup functions
	Card *setup(Builder *builder);
	void clear();
	void update();

	//General interaction methods
	bool grab(unsigned int num, unc from, unc user);
	bool test(unc to, unc user);
	bool place(unc to, unc user);

	//Clear hand
	void cancel(unc user);

	//History management
	void undo(unc user);
	void redo(unc user);
	Move *get_current() {
		return current;
	}

	//File loading/saving
	void save(string file);

	//Stack access
	int get_stack_count() {
		return STACKCOUNT;
	}
	Stack *get_stack(unc i) {
		if(i >= STACKCOUNT)
			return NULL;
		return &stack[i];
	}
	unsigned int cards_remaining() {
		return cardsLeft;
	}
};
