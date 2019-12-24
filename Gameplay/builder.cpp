#include "builder.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using std::bitset;

//Ensure consistant tag data structure
sexpr Builder::tag_eval(cell const &c) {
	sexpr list = env.list_eval(c);
	if(list[0].type == DECK) {
		sexpr *output = new sexpr();
		output->push_back(cell(env.filter_eval(c), FILTER));
		return *output;
	}
	if(c.type == EXPR) {
		sexpr *output = new sexpr();
		output->push_back(c);
		return *output;
	}
	return list;
}

//Create card from lisp cell
Card *Builder::make_card(const cell &source) {
	Card *current = NULL;
	Card *start = NULL;
	sexpr deck = env.deck_eval(source);
	for(cell card : deck) {
		//Initial card
		if(current == NULL) {
			current = new Card(env.card_eval(card));
			start = current;
		} else {
			//Add to list
			Card *value = new Card(env.card_eval(card));
			current->set_next(value);
			current = value;
		}
	}
	return start;
}

//Set internal values of stack
layout Builder::make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y) {
	layout dim = {1, 1, 1};
	stack.set_cords(x, y);

	//Set base type tags
	switch(type) {
		case HStack:
			stack.set_tag(SPREAD);
			stack.set_tag(SPREAD_HORIZONTAL);
			dim.x = 3;
			break;
		case VStack:
			stack.set_tag(SPREAD);
			dim.y = 5;
			break;
		default:
			break;
	}

	//Read connected tags
	for(cell c : data) {
		if(c.type == FILTER) {
			std::cout << "\tFilter by: " << env.str_eval(c, true) << "\n";
		} else if(c.type == EXPR) {
			//Special tag evaluation
			sexpr list = std::get<sexpr>(c.content);
			if(env.str_eval(list[0]) == "Start") {
				stack.set_start(env.num_eval(list[1]), env.num_eval(list[2]));
			}
		} else {
			//Boolean tag evaluation
			auto tag = tag_map.find(env.str_eval(c));
			if(tag != tag_map.end()) {
				//Base boolean tag
				stack.set_tag(tag->second);
			} else if(env.str_eval(c, false) == "Start-Extra") {
				std::cout << "\tStart-Extra\n";
				stack.set_start(-1, 0);
			} else {
				std::cout << "Not regular tag: " << env.str_eval(c, true) << " type: " << c.type << "\n";
			}
		}
	}
	return dim;
}

//Recursively build stack objects from lisp structure
layout Builder::make_layout(Solisp::Stack *stack, cell layout_c, sexpr tags, layout current) {
	//std::cout << "Next layer = " << str_eval(layout, true) << "\n";
	sexpr list = env.layout_eval(layout_c);
	sexpr array;
	layout added;
	layout final;

	int type = env.num_eval(list[0]);
	switch(type) {
		//General linear layouts
		case VLayout: case HLayout:
			for(int i = 1; i < list.size(); i++) {
				do {
					added = make_layout(stack, list[i], tags, current);

					//Keep track of position and count
					current.count = added.count;
					if(type == VLayout) {
						//Sum y and max x
						current.y += added.y;
						final.y = current.y;

						if(final.x - current.x < added.x)
							final.x = added.x + current.x;
					} else {
						//Sum x and max y
						current.x += added.x;
						final.x = current.x;
						if(final.y - current.y < added.y)
							final.y = added.y + current.y;
					}

					if(added.recurse != -1)
						current.recurse = added.recurse - 1;
				} while(current.recurse > 0);
				current.recurse = -1;
			}
			final.count = current.count;
			return final;
		//Basic card slots
		case Slot: case HStack: case VStack:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());

			std::cout << "Slot " << current.count << ":\n";

			added = make_slot(stack[current.count], tags, env.num_eval(list[0]), current.x, current.y);
			added.count += current.count;
			added.recurse = current.recurse;
			return added;
		//Apply tags to all slots in layout
		case Apply:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			return make_layout(stack, list[2], tags, current);
		case Multiply:
			if(current.recurse < 0)
				current.recurse = env.num_eval(list[1]);
			return make_layout(stack, list[2], tags, current);
	}
	return current;
}

//Get the overall deck to play with
Card *Builder::get_deck() {
	Card *c = make_card(env.read_stream(rule_file, DECK));
	std::cout << "Deck loaded\n";
	return c;
}

//Set up all stacks on game board
int Builder::set_stacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	std::cout << "Slot 0: \n";
	make_slot(stack[0], tag_eval(env.read_stream(rule_file, LIST)), VStack, -1, -1);
	return make_layout(stack, env.read_stream(rule_file, LAYOUT)).count;
}	