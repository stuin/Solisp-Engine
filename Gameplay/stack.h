#pragma once

namespace Solisp {
	class Stack;
}

#include <bitset>
#include <map>

#include "filter.h"

/*
 * Created by Stuart Irwin on 6/9/2019.
 * Individual slot for cards
 */

using std::bitset;

#define STACKTAGCOUNT 10
#define STACKFUNCOUNT 6

//List of tags to change stack properties
enum stack_tags { GOAL, INPUT, OUTPUT, SPREAD,
	SPREAD_HORIZONTAL, SPREAD_REVERSE, SPREAD_FAKE, BUTTON, SWAP, CUSTOM };
enum func_tag { ONGRAB, ONPLACE, ONFLIP, ONSTART, GRABIF, PLACEIF };

class Solisp::Stack {
private:
	//Stack properties
	Filter *filter = NULL;
	bitset<STACKTAGCOUNT> tags;
	void *functions[STACKFUNCOUNT];
	unsigned int max = 0;

	//Current state
	Card *stack = NULL;
	unsigned int count = 0;

public:
	//Starting count
	int start_hidden = 0;
	int start_shown = 0;

	int x = -1;
	int y = -1;
	int width = -1;
	int height = -1;

	static std::map<string, stack_tags> tag_map;
	static std::map<string, func_tag> func_map;

	Stack();
	Stack(const Stack &other) {
		//Game defined variables
		filter = other.filter;
		tags = other.tags;
		//functions = other.functions;
		max = other.max;

		//Game start position
		start_hidden = other.start_hidden;
		start_shown = other.start_shown;
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		//Current game state
		stack = other.stack->clone();
		full_count();
	}
	~Stack() {
		delete filter;
		delete stack;
	}

	//Unique variable setters
	void set_cords(int x, int y, int width, int height) {
		this->x = x;
		this->y = y;
		this->width = x + width;
		this->height = y + height;
	}
	void set_tag(int tag, bool value=true) {
		tags[tag] = value;
	}
	void set_start(int hidden=0, int shown=0) {
		start_hidden = hidden;
		start_shown = shown;
	}
	void add_count(unsigned int count) {
		this->count += count;
		if(this->count > 1000000)
			this->count = 0;
	}

	//General variable setters
	void set_max(unsigned int max) {
		this->max = max;
	}
	void set_card(Card *card) {
		stack = card;
	}
	void set_filter(Filter *filter) {
		this->filter = filter;
	}

	void set_function(void *function, func_tag type);
	bool run_function(func_tag func, unsigned char first, unsigned char second);

	//Count all cards in list
	void full_count() {
		Card *c = stack;
		count = 0;
		while(c != NULL) {
			count++;
			c = c->get_next();
		}
	}

	//Check if new cards can be placed on stack
	bool matches(unsigned int count, Card *newCard) {
		//Check if INPUT or if inserting multiple into single stack
		if(tags[INPUT] || (!tags[SPREAD] && !tags[SPREAD_FAKE] && count > 1))
			return false;

		//Check max card count of stack
		if(max > 0 && this->count + count > max)
			return false;

		//return true;
		return filter == NULL || filter->matches(count, newCard, stack);
	}

	//Get tag value
	bool get_tag(int tag) {
		if(tag >= 0 && tag < STACKTAGCOUNT)
			return tags[tag];
		return false;
	}

	//General variable getters
	unsigned int get_max() {
		return max;
	}
	unsigned int get_count() {
		return count;
	}
	Card *get_card() {
		return stack;
	}
};
