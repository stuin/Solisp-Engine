#include "lisp.h"
#include "../Gameplay/builder.h"
#include "../Gameplay/move.h"

/*
 * Created by Stuart Irwin on 28/1/2020.
 * Lisp structures for moving cards during game
 */

using Solisp::Move;

#define genv ((GameEnviroment*)env)

class GameEnviroment : public Enviroment {
private:
	//Generate base functions
	cell general_move(int num, bool flip, bool player);
	cell soft_move(int num);

	void build_library_game();

	char STACKCOUNT;
	Solisp::Stack *stacks;
	Move *current;

public:
	//Check if both slots in range
	bool both_valid(int from, int to) {
		return from > 0 && to > 0 &&
			from < STACKCOUNT && to < STACKCOUNT;
	}

	//Check if move matches filters
	bool test_move(int count, int from, int to) {
		return stacks[to].matches(count, stacks[from].get_card());
	}

	//Add new move to game
	void add_move(int count, int from, int to, bool player, bool flip) {
		*current += new Move(count, from, to, player, flip, current);
	}

	//Count stack
	int get_count(int i) {
		return stacks[i].get_count();
	}

	//Set this value before evaluating
	bool run(cell c, int stack, Move *current) {
		this->current = current;
		set("this", stack);
		return num_eval(c);
	}

	void setup(Solisp::Stack *stacks, int STACKCOUNT) {
		this->STACKCOUNT = STACKCOUNT;
		this->stacks = stacks;

		build_library_game();
		shift_env(true);
	}
};