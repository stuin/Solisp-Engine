#include "builder.h"
#include "../Lisp/card_env.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using Solisp::Stack;
using Solisp::Filter;
using std::bitset;
using std::cout;

CardEnviroment builder_env;

std::map<string, stack_tags> Stack::tag_map = {
	{"GOAL", GOAL},
	{"INPUT", INPUT},
	{"OUTPUT", OUTPUT},
	{"BUTTON", BUTTON},
	{"MIRRORED", SPREAD_REVERSE},
	{"MULTI", SPREAD_FAKE},
	{"CUSTOM", CUSTOM}
};
std::map<string, func_tag> Stack::func_map = {
	{"On-Grab", ONGRAB},
	{"On-Place", ONPLACE},
	{"On-Flip", ONFLIP},
	{"On-Start", ONSTART},
	{"Grab-If", GRABIF},
	{"Place-If", PLACEIF}
};

//Ensure consistant tag data structure
sexpr Builder::tag_eval(sexpr list, bool layout) {
	sexpr output;
	if(layout)
		list.erase(list.begin());

	if(list[0].type == FILTER) {
		output.push_back(cell(list, TAGFILTER));
		return output;
	}
	return list;
}

//Create card from lisp cell
Card *Builder::make_card(const cell &source, bool shuffled) {
	Card *current = NULL;
	Card *start = NULL;
	sexpr deck = builder_env.deck_eval(source);
	//shuffled = false;

	for(unsigned int i = deck.size(); i > 0; i--) {
		cell card = deck[i - 1];

		//Initial card
		if(current == NULL) {
			current = new Card(builder_env.card_eval(card));
			start = current;
		} else if(!shuffled) {
			//Add to list
			Card *value = new Card(builder_env.card_eval(card));
			current->set_next(value);
			current = value;
		} else {
			Card *value = new Card(builder_env.card_eval(card));
			start = start->shuffle(value);
		}
	}
	return start;
}

//Create filter from lisp cell
Filter *Builder::make_filter(const cell &source) {
	//Retrieve cell values
	sexpr array = builder_env.tagfilter_eval(source, OPEN);
	filter_type open = (filter_type)builder_env.num_eval(array[1]);
	array = builder_env.filter_eval(array[0]);
	Filter *output = new Filter(open);

	//cout << "\tFilter by: " << builder_env.str_eval(cell(array, FILTER), true) << "with code: " << open << "\n";

	//Pass each deck to filter
	for(cell deck : array)
		*output += make_card(deck, false);

	return output;
}

//Recursively build stack objects from lisp structure
layout Builder::make_layout(Solisp::Stack *stack, cell layout_c, sexpr tags, layout current) {
	//cout << "Next layer = " << str_eval(layout, true) << "\n";
	sexpr list = builder_env.layout_eval(layout_c);
	sexpr array;
	layout added;
	layout final;

	int type = builder_env.num_eval(list[0]);
	switch(type) {
		//General linear layouts
		case VLayout: case HLayout:
			for(unsigned int i = 1; i < list.size(); i++) {
				added = make_layout(stack, list[i], tags, current);

				//Keep track of position and count
				current.count = added.count;
				if(type == VLayout) {
					//Sum y and max x
					current.y += added.y;
					final.y = current.y;

					if(final.x < added.x)
						final.x = added.x;
				} else {
					//Sum x and max y
					current.x += added.x;
					final.x = current.x;
					if(final.y < added.y) {
						final.y = added.y;
					}
				}
				//cout << "\tEnd Height value: " << current.y << "+" << added.y << "\n";
			}

			final.count = current.count;
			return final;
		//Basic card slots
		case Slot: case HStack: case VStack:
			array = tag_eval(list, true);
			array.insert(array.end(), tags.begin(), tags.end());

			//cout << "Slot " << (int)current.count << ":\n";
			try {
				added = make_slot(stack[current.count], array, builder_env.num_eval(list[0]), current.x, current.y);
			} catch(std::exception &e) {
				std::cerr << "Slot Error: " << e.what() << std::endl;
				std::cerr << builder_env.str_eval(cell(array)) << "\n";
			}
			added.count += current.count;
			return added;
		//Apply tags to all slots in layout
		case Apply:
			array = tag_eval(builder_env.list_eval(list[1]), false);
			tags.insert(tags.end(), array.begin(), array.end());
			return make_layout(stack, list[2], tags, current);
	}
	return current;
}

//Set internal values of stack
layout Builder::make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y) {
	layout dim = {1, 2, 1};
	stack.set_cords(x, y);

	//Read connected tags
	for(cell c : data) {
		//cout << "cell: " << builder_env.str_eval(c, true) << "\n";
		if(c.type == TAGFILTER || c.type == FILTER)
			stack.set_filter(make_filter(c));
		else if(c.type == EXPR) {
			//Special tag evaluation
			sexpr list = std::get<sexpr>(c.content);
			auto func = Stack::func_map.find(builder_env.str_eval(list[0]));

			if(func != Stack::func_map.end()) {
				stack.set_function(list, func->second);
				//cout << "\tFunction set: " << builder_env.str_eval(c, true) << "\n";
			} else if(builder_env.str_eval(list[0]) == "Start")
				stack.set_start(builder_env.num_eval(list[1]), builder_env.num_eval(list[2]));
			else if(builder_env.str_eval(list[0]) == "Max")
				stack.set_max(builder_env.num_eval(list[1]));
			else if(builder_env.str_eval(list[0], true).find("Filter") == 0)
				stack.set_filter(make_filter(c));
			else
				cout << "\tUnhandled expression: " << builder_env.str_eval(c, true) << "\n";
		} else {
			//Boolean tag evaluation
			auto tag = Stack::tag_map.find(builder_env.str_eval(c));
			if(tag != Stack::tag_map.end()) {
				//Base boolean tag
				stack.set_tag(tag->second);
			} else if(builder_env.str_eval(c, false) == "Start-Extra") {
				//cout << "\tStart-Extra\n";
				stack.set_start(-1, 0);
			} else
				cout << "Unhandled tag: " << builder_env.str_eval(c, true) << "\n";
		}
	}

	//Set base type tags
	switch(type) {
		case HStack:
			stack.set_tag(SPREAD);
			stack.set_tag(SPREAD_HORIZONTAL);
			dim.x = 7;

			//Adjust for maximum
			if(stack.get_max() != 0)
				dim.x = stack.get_max();
			break;
		case VStack:
			stack.set_tag(SPREAD);
			dim.y = 7;

			//Adjust for maximum
			if(stack.get_max() != 0)
				dim.y = stack.get_max();
			break;
		default:
			break;
	}

	dim.x += 1;
	dim.y += 2;
	return dim;
}

//Get the overall deck to play with
Card *Builder::get_deck() {
	Card *c = make_card(builder_env.read_stream(rule_file, DECK), true);
	cout << "Deck loaded\n";
	return c;
}

//Set up all stacks on game board
int Builder::set_stacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);
	cell c;

	try {
		//std::cout << "Slot 0: \n";
		c = builder_env.read_stream(rule_file, EXPR);

		sexpr array;
		array.push_back(cell("VStack"));
		array.push_back(c);
		make_slot(stack[0], tag_eval(builder_env.layout_eval(array), true), VStack, -1, -1);

		c = builder_env.read_stream(rule_file, LAYOUT);
		return make_layout(stack, c).count;
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		//std::cerr << builder_env.str_eval(c, true) << "\n";
	}
	return 0;
}

//Delete builder
Builder::~Builder() {
	rule_file.close();
}