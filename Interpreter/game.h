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
	char STACKCOUNT = 10;

	//Current game state
	Stack stack[MAXSTACKCOUNT];
	Move *current;

	//Current move
	int from = -1;
	int to = -1;
	int count = 0;

	//Apply current moves to stack array
	void apply();
	void apply(Move *move, bool reverse);

	//Run functions and check win
	void update();

	//Startup functions
	void shuffle();
	void deal();

public:

	Game() {}

	//Setup functions
	Card *setup(Builder *builder);

	//Pick up cards from stack
	bool grab(int num, int from);

	//Clear hand
	void cancel();

	void undo();
	void redo();
};
