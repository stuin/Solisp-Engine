namespace Solisp {
	class Stack;
}

#include <bitset>

#include "filter.h"

/*
 * Created by Stuart Irwin on 6/9/2019.
 * Individual slot for cards
 */

using std::bitset;

#define MAXSTACKCOUNT 30
#define STACKTAGCOUNT 8

enum stack_tags { GOAL, INPUT, OUTPUT, SPREAD,
	SPREAD_HORIZONTAL, SPREAD_REVERSE, BUTTON, CUSTOM };

class Solisp::Stack {
private:
	//Stack properties
	Filter *filter = NULL;
	bitset<STACKTAGCOUNT> tags;
	int max = -1;

	//Current state
	Card *stack = NULL;
	int count = 0;

public:
	//Starting count
	int start_hidden = 0;
	int start_shown = 0;

	int x = -1;
	int y = -1;

	Stack() {}
	~Stack() {
		delete filter;
		delete stack;
	}

	void set_cords(int x, int y) {
		this->x = x;
		this->y = y;
	}

	void set_tag(int tag, bool value=true) {
		tags[tag] = value;
	}

	//Set maximum card count
	void set_max(int max) {
		this->max = max;
	}

	void set_start(int hidden=0, int shown=0) {
		start_hidden = hidden;
		start_shown = shown;
	}

	void add_count(int count) {
		this->count += count;
	}

	//Set top of stack
	void set_card(Card *card) {
		stack = card;
	}

	void set_filter(Filter *filter) {
		this->filter = filter;
	}

	//Check if new cards can be placed on stack
	bool matches(int count, Card *newCard) {
		//Check if INPUT or if inserting multiple into single stack
		if(tags[INPUT] || (!tags[SPREAD] && count > 1))
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

	int get_max() {
		return max;
	}

	//Get number of cards in stack
	int get_count() {
		return count;
	}

	//Get top card of stack
	Card *get_card() {
		return stack;
	}
};
