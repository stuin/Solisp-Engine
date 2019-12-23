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

	//Map string values to tag codes
	std::map<string, stack_tags> tag_map = {
		{"GOAL", GOAL},
		{"INPUT", INPUT},
		{"OUTPUT", OUTPUT},
		{"BUTTON", BUTTON},
		{"CUSTOM", CUSTOM}
	};
	sexpr tag_eval(cell const &c);

	Card *make_card(const cell &source);
	layout make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y);
	layout make_layout(Solisp::Stack *stack, cell layout, sexpr tags={}, struct layout current={0,0,1});


public:

	Builder() {
		rule_file.open("/media/Programming/C/godot/modules/solisp/Games/Klondike.solisp");
		std::cout << "Builder initialized \n";
	}

	//Get full list of cards at start
	Card *get_deck();

	//Initialize stacks and return stack count
	int set_stacks(Stack *stacks);
};
