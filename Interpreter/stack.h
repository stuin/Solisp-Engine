#include <bitset>

#include "filter.h"

/*
 * Created by Stuart Irwin on 6/9/2019.
 * Individual slot for cards
 */

using std::bitset;

#define STACKTAGCOUNT 6

class Stack {
private:
	Card *stack;
	Filter *filter = NULL;
	bitset<STACKTAGCOUNT> tags; //GOAL, INPUT, OUTPUT, SPREAD, BUTTON
	int count;
	int max;

public:
	Stack(bitset<STACKTAGCOUNT> tags) {
		this->tags = tags;
	}

	//Check if new cards can be placed on stack
	bool matches(int count, Card *newCard) {
		//Check if INPUT or if inserting multiple into single stack
		if(tags[1] || (!tags[3] && count > 1))
			return false;

		//Check max card count of stack
		if(max > 0 && this->count + count > max)
			return false;

		return filter != NULL || filter->matches(count, newCard, stack);
	}

	//Get tag value
	bool getTag(int tag) {
		if(tag > 0 && tag < STACKTAGCOUNT)
			return tags[tag];
		return false;
	}

	//Get number of cards in stack
	int getCount() {
		return count;
	}

	//Get top card of stack
	Card *getCard() {
		return stack;
	}

	//Set top of stack
	void setCard(Card *card) {
		stack = card;
	}
};