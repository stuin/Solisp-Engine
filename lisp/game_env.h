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
		cout << "Moving " << count << " cards from " << from << " to " << to << "\n";
		*current += new Move(count, from, to, player, flip, current);
	}

	//Count stack
	int get_count(int i) {
		cout << "Counting cards\n";
		return stacks[i].get_count();
	}

	//Set this value before evaluating
	bool run(cell c, int stack, Move *current) {
		this->current = current;
		set("this", stack);
		bool output = true;

		try {
			output = num_eval(c);
		} catch(std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << str_eval(c, true) << "\n";
		}

		return output;
	}

	//Build up lisp enviroment
	void setup(Solisp::Stack *stacks, int STACKCOUNT) {
		this->STACKCOUNT = STACKCOUNT;
		this->stacks = stacks;

		//Build tag stack lists
		sexpr tags[STACKTAGCOUNT];
		for(int t = 0; t < STACKTAGCOUNT; t++) {
			for(int i = 0; i < STACKCOUNT; i++)
				if(stacks[i].get_tag(t))
					tags[t].push_back(cell(i));
		}

		//Link strings to stack tags
		auto it = Solisp::Builder::tag_map.begin();
		while(it != Solisp::Builder::tag_map.end()) {
			set(it->first, tags[it->second]);
			it++;
		}

		build_library_game();
		shift_env(true);
	}
};