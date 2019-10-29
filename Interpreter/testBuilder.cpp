#include "builder.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Static test definitions for Klondike Solitaire
 */

using Solisp::Builder;
using Solisp::Card;
using std::bitset;

Card *Builder::getDeck() {
	Card *output = new Card(1, 13, 'S');
	return output->fourSuit()->flatten();
}

int Builder::setStacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

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