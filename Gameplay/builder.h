namespace Solisp {
	class Builder;
}

#include "stack.h"

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Definitions for reading solisp file
 */

class Solisp::Builder {
public:
	//Get full list of cards at start
	Card *get_deck();

	//Initialize stacks and return stack count
	int set_stacks(Stack *stacks);
};
