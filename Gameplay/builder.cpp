#include "builder.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using std::bitset;

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
void Builder::make_slot(Solisp::Stack &stack, sexpr data, int type) {
	//Set base type tags
	switch(type) {
		case HStack:
			stack.set_tag(SPREAD);
			stack.set_tag(SPREAD_HORIZONTAL);
			break;
		case VStack:
			stack.set_tag(SPREAD);
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
				stack.set_start(env.num_eval(list[1]), env.num_eval(2));
			}
		} else {
			//Simple tag evaluation
			auto tag = tag_map.find(env.str_eval(c));
			if(tag != tag_map.end()) {
				stack.set_tag(tag->second);
			} else if(env.str_eval(c, false) == "Start-Extra") {
				std::cout << "\tStart-Extra\n";
				stack.set_start(-1, 0);
			} else {
				std::cout << "Not regular tag: " << env.str_eval(c, true) << " type: " << c.type << "\n";
			}
		}
	}
}

//Recursively build stack objects from lisp structure
int Builder::make_layout(Solisp::Stack *stack, cell layout, int index, sexpr tags) {
	//std::cout << "Next layer = " << str_eval(layout, true) << "\n";
	sexpr list = env.layout_eval(layout);
	sexpr array;
	switch(env.num_eval(list[0])) {
		case VLayout: case HLayout:
			for(int i = 1; i < list.size(); i++)
				index = make_layout(stack, list[i], index, tags);
			return index;
		case Slot: case HStack: case VStack:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			std::cout << "Slot " << index << "\n";
			make_slot(stack[index], tags, env.num_eval(list[0]));
			return index + 1;
		case Apply:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			return make_layout(stack, list[2], index, tags);
	}
	return index;
}

Card *Builder::get_deck() {
	std::cout << "Loading deck \n";
	std::cout << "File: " << rule_file.good() << "\n";
	Card *c = make_card(env.read_stream(rule_file, DECK));
	return c;
}

int Builder::set_stacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	std::cout << "Loading stacks \n";
	make_slot(stack[0], tag_eval(env.read_stream(rule_file, LIST)), VStack);
	return make_layout(stack, env.read_stream(rule_file, LAYOUT));
}	