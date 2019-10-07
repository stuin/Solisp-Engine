#include "builder.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using std::bitset;

Card *Builder::getDeck() {
	Card *output = new Card(1, 13, 'N');
	return output->fourSuit()->flatten();
}

int Builder::setStacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	//Draw pile
	bits[4] = 1;
	stack[0].setTags(bits, -1);
	bits.reset();

	//Drawn cards
	bits[1] = 1;
	bits[3] = 1;
	stack[1].setTags(bits);
	bits.reset();

	//Suits
	bits[0] = 1;
	bits[2] = 1;
	for(int i = 2; i < 6; i++)
		stack[i].setTags(bits);
	bits.reset();

	//Field
	bits[5] = 1;
	for(int i = 6; i < 13; i++)
		stack[i].setTags(bits, i - 6, 1);
	bits.reset();

	return 13;
}	