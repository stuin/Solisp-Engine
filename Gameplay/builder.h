namespace Solisp {
	class Builder;
}

#include "stack.h"
#include "../lisp/card_env.h"

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Definitions for reading solisp file
 */

struct layout {
	int x = 1;
	int y = 1;
	int count = 1;
};

class Solisp::Builder {
private:
	std::ifstream rule_file;
	CardEnviroment env;
	std::string name;

	//Map string values to tag codes
	std::map<string, stack_tags> tag_map = {
		{"GOAL", GOAL},
		{"INPUT", INPUT},
		{"OUTPUT", OUTPUT},
		{"BUTTON", BUTTON},
		{"MIRRORED", SPREAD_REVERSE},
		{"MULTI", SPREAD_FAKE},
		{"CUSTOM", CUSTOM}
	};
	sexpr tag_eval(sexpr list, bool layout);

	//Solisp to gameplay convertions
	Card *make_card(const cell &source, bool shuffled);
	Filter *make_filter(const cell &source);

	//Layout interpreting functions
	layout make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y);
	layout make_layout(Solisp::Stack *stack, cell layout, sexpr tags={}, struct layout current={0,0,1});


public:

	Builder(std::string file) {
		rule_file.open(file);
		std::getline(rule_file, name);
		std::cout << "Builder initialized with game: " << name << "\n";
	}

	//Get the overall deck to play with
	Card *get_deck() {
		Card *c = make_card(env.read_stream(rule_file, DECK), true);
		cout << "Deck loaded\n";
		return c;
	}

	//Set up all stacks on game board
	int set_stacks(Stack *stack) {
		bitset<STACKTAGCOUNT> bits(0);
		cell c;

		try {
			std::cout << "Slot 0: \n";
			c = env.read_stream(rule_file, EXPR);

			sexpr array;
			array.push_back(cell("VStack"));
			array.push_back(c);
			make_slot(stack[0], tag_eval(env.layout_eval(array), true), VStack, -1, -1);

			c = env.read_stream(rule_file, LAYOUT);
			return make_layout(stack, c).count;
		} catch(std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			//std::cerr << env.str_eval(c, true) << "\n";
		}
		return 0;
	}
};
