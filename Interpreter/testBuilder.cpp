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
	return output->fourSuit();
}

int Builder::setStacks(Stack *stack) {
	bitset<STACKTAGCOUNT> bits(0);

	//Hand
	bits[3] = 1;
	stack[0].setTags(bits);
	bits.reset();

	//Draw pile
	bits[1] = 1;
	stack[1].setTags(bits, -1);
	bits.reset();

	//Drawn cards
	//bits[1] = 1;
	bits[3] = 1;
	stack[2].setTags(bits);
	bits.reset();

	//Suits
	bits[0] = 1;
	bits[2] = 1;
	for(int i = 3; i < 7; i++)
		stack[i].setTags(bits);
	bits.reset();

	//Field
	bits[3] = 1;
	bits[5] = 1;
	for(int i = 7; i < 14; i++)
		stack[i].setTags(bits, 13 - i, 1);
	bits.reset();

	return 14;
}	