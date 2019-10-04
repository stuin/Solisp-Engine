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
	Card *getDeck();

	//Initialize stacks and return stack count
	int setStacks(Stack *stacks);

	//Read all card layouts
	Layout *getLayout();
};
