#include "builder.h"
#include "../lisp/card_library.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using std::bitset;

std::ifstream ruleFile("../Games/Klondike.solisp");

void SetupCards() {
	build_library();
}

//Create card from lisp cell
Card *MakeCard(const cell &source) {
	Card *current = NULL;
	Card *start = NULL;
	sexpr deck = deck_eval(source);
	for(cell card : deck) {
		//Initial card
		if(current == NULL) {
			current = new Card(card_eval(card));
			start = current;
		} else {
			//Add to list
			Card *value = new Card(card_eval(card));
			current->setNext(value);
			current = value;
		}
	}
	return start;
}

sexpr tag_eval(cell const &c) {
	sexpr list = list_eval(c);
	if(list[0].type == DECK) {
		sexpr *output = new sexpr();
		output->push_back(cell(filter_eval(c), FILTER));
		return *output;
	}
	if(c.type == EXPR) {
		sexpr *output = new sexpr();
		output->push_back(c);
		return *output;
	}
	return list;
}

//Map string values to tag codes
std::map<string, stack_tags> tagMap = {
	{"GOAL", GOAL},
	{"INPUT", INPUT},
	{"OUTPUT", OUTPUT},
	{"BUTTON", BUTTON},
	{"CUSTOM", CUSTOM}
};

//Set internal values of stack
void MakeSlot(Solisp::Stack &stack, sexpr data, int type) {
	//Set base type tags
	switch(type) {
		case HStack:
			stack.setTag(SPREAD);
			stack.setTag(SPREAD_HORIZONTAL);
			break;
		case VStack:
			stack.setTag(SPREAD);
			break;
		default:
			break;
	}

	//Read connected tags
	for(cell c : data) {
		if(c.type == FILTER) {
			std::cout << "Filter by: " << str_eval(c, true) << "\n";
		} else if(c.type == EXPR) {
			//Special tag evaluation
			sexpr list = std::get<sexpr>(c.content);
			if(str_eval(list[0]) == "Start") {
				stack.setStart(num_eval(list[1]), num_eval(2));
			}
		} else {
			//Simple tag evaluation
			auto tag = tagMap.find(str_eval(c));
			if(tag != tagMap.end()) {
				stack.setTag(tag->second);
			} else {
				std::cout << "Not regular tag: " << str_eval(c, true) << " type: " << c.type << "\n";
			}
		}
	}
}

//Recursively build stack objects from lisp structure
int MakeLayout(Solisp::Stack *stack, cell layout, int index=0, sexpr tags={}) {
	//std::cout << "Next layer = " << str_eval(layout, true) << "\n";
	sexpr list = layout_eval(layout);
	sexpr array;
	switch(num_eval(list[0])) {
		case VLayout: case HLayout:
			for(int i = 1; i < list.size(); i++)
				index = MakeLayout(stack, list[i], index, tags);
			return index;
		case Slot: case HStack: case VStack:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			MakeSlot(stack[index], tags, num_eval(list[0]));
			return index + 1;
		case Apply:
			array = tag_eval(list[1]);
			tags.insert(tags.end(), array.begin(), array.end());
			return MakeLayout(stack, list[2], index, tags);
	}
	return index;
}

Card *Builder::getDeck() {
	SetupCards();
	Card *c = MakeCard(read_stream(ruleFile, DECK));
	return c;
}

int Builder::setStacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	std::cout << "Lisp inputs \n";
	MakeSlot(stack[0], tag_eval(read_stream(ruleFile, LIST)), VStack);
	return MakeLayout(stack, read_stream(ruleFile, LAYOUT));

	//Hand
	bits[SPREAD] = 1;
	stack[0].setTags(bits);
	bits.reset();

	//Draw pile
	bits[INPUT] = 1;
	bits[BUTTON] = 1;
	stack[1].setTags(bits, -1);
	bits.reset();

	//Drawn cards
	//bits[INPUT] = 1;
	bits[SPREAD] = 1;
	bits[SPREAD_HORIZONTAL] = 1;
	stack[2].setTags(bits);
	bits.reset();

	//Suits
	bits[GOAL] = 1;
	bits[OUTPUT] = 1;
	for(int i = 3; i < 7; i++)
		stack[i].setTags(bits);
	bits.reset();

	//Field
	bits[SPREAD] = 1;
	bits[CUSTOM] = 1;
	for(int i = 7; i < 14; i++)
		stack[i].setTags(bits, 13 - i, 1);
	bits.reset();

	return 14;
}	