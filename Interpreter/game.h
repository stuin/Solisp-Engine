namespace Solisp {
	class Game;
}

#include <vector>
#include <queue>

#include "builder.h"
#include "move.h"

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

class Solisp::Game {
private:
	//Game definition
	//Feature functions;
	char STACKCOUNT = MAXSTACKCOUNT;

	//Current game state
	Stack stack[MAXSTACKCOUNT];
	Move *current = new Move(0, 0, 0, false, false, NULL);

	//Current move
	int from = -1;
	int to = -1;
	int count = 0;
	int tested = -1;

	//Apply current moves to stack array
	void apply();
	void apply(Move *move, bool reverse);

	//Startup functions
	void shuffle();
	void deal();

public:

	Game() {}
	~Game() {
		current->clearForward();
		delete current;
	}

	//Setup functions
	Card *setup(Builder *builder);
	void update();

	//General place method
	bool grab(int num, int from);
	bool test(int to);
	bool place(int to);

	//Clear hand
	void cancel();

	void undo();
	void redo();

	Stack *getStack(int i) {
		if(i >= STACKCOUNT)
			return NULL;
		return &stack[i];
	}
};
