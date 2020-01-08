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

//Ensure consistant tag data structure
sexpr Builder::tag_eval(cell const &c) {
	sexpr list = env.list_eval(c);
	if(list[0].type == FILTER) {
		sexpr *output = new sexpr();
		output->push_back(env.force_eval[TAGFILTER](&env, c));
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
Card *Builder::make_card(const cell &source, bool shuffled) {
	Card *current = NULL;
	Card *start = NULL;
	sexpr deck = env.deck_eval(source);

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
	sexpr array = env.tagfilter_eval(source);
	bool open = env.num_eval(array[1]);
	array = env.filter_eval(array[0]);
	Filter *output = new Filter(open);

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
			for(int i = 1; i < list.size(); i++) {
				do {
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

					//Check if current layout contains multiplier
					if(added.recurse != -1)
						current.recurse = added.recurse - 1;
				} while(current.recurse > 0);
				current.recurse = -1;

				//cout << "\tEnd Height value: " << current.y << "+" << added.y << "\n";
			}

			final.count = current.count;
			return final;
		//Basic card slots
		case Slot: case HStack: case VStack:
			array = tag_eval(list[1]);
			array.insert(array.end(), tags.begin(), tags.end());

			cout << "Slot " << current.count << ":\n";

			added = make_slot(stack[current.count], array, env.num_eval(list[0]), current.x, current.y);
			added.count += current.count;
			added.recurse = current.recurse;
			return added;
		//Apply tags to all slots in layout
		case Apply:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			return make_layout(stack, list[2], tags, current);
		//Tell parent to duplicate internal layout
		case Multiply:
			if(current.recurse < 0) {
				added = make_layout(stack, list[2], tags, current);
				added.recurse = env.num_eval(list[1]);
			} else {
				int transfer = current.recurse;
				current.recurse = -1;
				added = make_layout(stack, list[2], tags, current);
				added.recurse = transfer;
			}
			return added;
	}
	return current;
}

//Set internal values of stack
layout Builder::make_slot(Solisp::Stack &stack, sexpr data, int type, int x, int y) {
	layout dim = {1, 1, 1, -1};
	stack.set_cords(x, y);

	//Read connected tags
	for(cell c : data) {
		if(c.type == TAGFILTER || c.type == FILTER) {
			cout << "\tFilter by: " << env.str_eval(c, true) << "\n";
			stack.set_filter(make_filter(c));
		} else if(c.type == EXPR) {
			//Special tag evaluation
			sexpr list = std::get<sexpr>(c.content);
			if(env.str_eval(list[0]) == "Start")
				stack.set_start(env.num_eval(list[1]), env.num_eval(list[2]));
			else if(env.str_eval(list[0]) == "Max")
				stack.set_max(env.num_eval(list[1]));
			else if(env.str_eval(list[0]).find("Filter") == 0) {
				cout << "\tFilter by: " << env.str_eval(c, true) << "\n";
				stack.set_filter(make_filter(c));
			} //else
				//cout << "\tUnhandled expression: " << env.str_eval(c, true) << "\n";
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

//Get the overall deck to play with
Card *Builder::get_deck() {
	Card *c = make_card(env.read_stream(rule_file, DECK), true);
	cout << "Deck loaded\n";
	return c;
}

//Set up all stacks on game board
int Builder::set_stacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	std::cout << "Slot 0: \n";
	make_slot(stack[0], tag_eval(env.read_stream(rule_file, LIST)), VStack, -1, -1);
	return make_layout(stack, env.read_stream(rule_file, LAYOUT)).count;
}