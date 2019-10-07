#include "Card.h"
#include "Filter.h"

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Card setup functions
 */

using Solisp::Card;
using Solisp::Filter;

//Constructor for full list of cards
Card::Card(char start, char end, char suit) {
	Card(start, suit);

	if(start < end)
		next = new Card(start+1, end, suit);
}

//Reverse full list of cards
Card *Card::reverse(Card *last) {
	if(next == NULL) {
		next = last;
		return this;
	}

	Card *other = next;
	next = last;
	return other->reverse(this);
}

//Copy entire list with different suit
Card *Card::withSuit(char suit) {
	Card *output = new Card(data.value, suit);
	if(next != NULL)
		output->setNext(next->withSuit(suit));
	return output;
}


//Create filter with all four suits
Filter *Card::fourSuit() {
	Filter *output = new Filter(withSuit('D'));
	*output += withSuit('C');
	*output += withSuit('H');
	*output += withSuit('S');
	return output;
}

//Create filter with two alternating lists
Filter *Card::alternating() {
	Filter *output = new Filter(alternating(true));
	*output += alternating(false);
	return output;
}

//Copy list with alternating colors
Card *Card::alternating(bool black) {
	Card *output = new Card(data.value, black ? 'B' : 'R');
	if(next != NULL)
		output->setNext(next->alternating(!black));
	return output;
}

//Add new card to list
void Card::operator+=(Card *next) {
	if(this->next == NULL)
		this->next = next;
	else
		*(this->next) += next;
}