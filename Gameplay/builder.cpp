#include "builder.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using Solisp::Filter;
using std::bitset;
using std::cout;

std::map<string, stack_tags> Builder::tag_map = {
	{"GOAL", GOAL},
	{"INPUT", INPUT},
	{"OUTPUT", OUTPUT},
	{"BUTTON", BUTTON},
	{"MIRRORED", SPREAD_REVERSE},
	{"MULTI", SPREAD_FAKE},
	{"CUSTOM", CUSTOM}
};
std::map<string, func_tag> Builder::func_map = {
	{"On-Grab", ONGRAB},
	{"On-Place", ONPLACE},
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
	sexpr deck = env.deck_eval(source);
	//shuffled = false;

	for(int i = deck.size(); i > 0; i--) {
		cell card = deck[i - 1];

		//Initial card
		if(current == NULL) {
			current = new Card(env.card_eval(card));
			start = current;
		} else if(!shuffled) {
			//Add to list
			Card *value = new Card(env.card_eval(card));
			current->set_next(value);
			current = value;
		} else {
			Card *value = new Card(env.card_eval(card));
			start = start->shuffle(value);
		}
	}
	return start;
}

//Create filter from lisp cell
Filter *Builder::make_filter(const cell &source) {
	//Retrieve cell values
	sexpr array = env.tagfilter_eval(source, OPEN);
	filter_type open = (filter_type)env.num_eval(array[1]);
	array = env.filter_eval(array[0]);
	Filter *output = new Filter(open);

	cout << "\tFilter by: " << env.str_eval(cell(array, FILTER)) << "with code: " << open << "\n";

	//Pass each deck to filter
	for(cell deck : array)
		*output += make_card(deck, false);

	return output;
}

//Recursively build stack objects from lisp structure
layout Builder::make_layout(Solisp::Stack *stack, cell layout_c, sexpr tags, layout current) {
	//cout << "Next layer = " << str_eval(layout, true) << "\n";
	sexpr list = env.layout_eval(layout_c);
	sexpr array;
	layout added;
	layout final;

	int type = env.num_eval(list[0]);
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

			cout << "Slot " << current.count << ":\n";
			try {
				added = make_slot(stack[current.count], array, env.num_eval(list[0]), current.x, current.y);
			} catch(std::exception &e) {
				std::cerr << "Slot Error: " << e.what() << std::endl;
				std::cerr << env.str_eval(cell(array)) << "\n";
			}
			added.count += current.count;
			return added;
		//Apply tags to all slots in layout
		case Apply:
			array = tag_eval(env.list_eval(list[1]), false);
			tags.insert(tags.end(), array.begin(), array.end());
			return make_layout(stack, list[2], tags, current);
	}
	return current;
}

//Set internal values of stack
layout Builder::make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y) {
	layout dim = {1, 1, 1};
	stack.set_cords(x, y);

	//Read connected tags
	for(cell c : data) {
		//cout << "cell: " << env.str_eval(c, true) << "\n";
		if(c.type == TAGFILTER || c.type == FILTER)
			stack.set_filter(make_filter(c));
		else if(c.type == EXPR) {
			//Special tag evaluation
			sexpr list = std::get<sexpr>(c.content);
			auto func = func_map.find(env.str_eval(list[0]));

			if(func != func_map.end()) {
				stack.set_function(list, func->second);
				cout << "\tFunction set: " << env.str_eval(c, true) << "\n";
			} else if(env.str_eval(list[0]) == "Start")
				stack.set_start(env.num_eval(list[1]), env.num_eval(list[2]));
			else if(env.str_eval(list[0]) == "Max")
				stack.set_max(env.num_eval(list[1]));
			else if(env.str_eval(list[0], true).find("Filter") == 0)
				stack.set_filter(make_filter(c));
			else
				cout << "\tUnhandled expression: " << env.str_eval(c, true) << "\n";
		} else {
			//Boolean tag evaluation
			auto tag = tag_map.find(env.str_eval(c));
			if(tag != tag_map.end()) {
				//Base boolean tag
				stack.set_tag(tag->second);
			} else if(env.str_eval(c, false) == "Start-Extra") {
				cout << "\tStart-Extra\n";
				stack.set_start(-1, 0);
			} else
				cout << "Unhandled tag: " << env.str_eval(c, true) << "\n";
		}
	}

	//Set base type tags
	switch(type) {
		case HStack:
			stack.set_tag(SPREAD);
			stack.set_tag(SPREAD_HORIZONTAL);
			dim.x = 7;

			//Adjust for maximum
			if(stack.get_max() != -1)
				dim.x = stack.get_max();
			break;
		case VStack:
			stack.set_tag(SPREAD);
			dim.y = 7;

			//Adjust for maximum
			if(stack.get_max() != -1)
				dim.y = stack.get_max();
			break;
		default:
			break;
	}

	dim.x += 1;
	dim.y += 3;
	return dim;
}