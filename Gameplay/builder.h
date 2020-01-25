namespace Solisp {
	class Builder;
}

#include "stack.h"
#include "../lisp/card_lisp.h"

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

	//Get full list of cards at start
	Card *get_deck();

	//Initialize stacks and return stack count
	int set_stacks(Stack *stacks);
};
