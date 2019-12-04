#include "Filter.h"

#include <iostream>

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Card setup functions
 */

using Solisp::Card;
using Solisp::Filter;

//Constructor for full list of cards
Card::Card(char start, char end, char suit) {
	data.value = start;
	data.suit = suit;

	if(start < end)
		next = new Card(start+1, end, suit);
}

//Copy list of cards
Card *Card::clone() {
	Card *output = new Card(data.value, data.suit);
	if(next != NULL)
		output->setNext(next->clone());
	return output;
}

//Reverse full list of cards
Card *Card::reverse(int index, Card *last) {
	this->index = index;

	//If final card
	if(next == NULL) {
		next = last;
		return this;
	}

	//If middle card
	Card *other = next;
	next = last;
	return other->reverse(index + 1, this);
}

//Set entire list to different suit
Card *Card::withSuit(char suit) {
	data.suit = suit;
	if(next != NULL)
		next->withSuit(suit);
	return this;
}

//Create filter with all four suits
Filter *Card::fourSuit() {
	Filter *output = new Filter(withSuit('D'));
	*output += clone()->withSuit('C');
	*output += clone()->withSuit('H');
	*output += clone()->withSuit('S');
	return output;
}

//Create filter with two alternating lists
Filter *Card::alternating() {
	Filter *output = new Filter(alternating(true));
	*output += clone()->alternating(false);
	return output;
}

//Set list to alternating colors
Card *Card::alternating(bool black) {
	data.suit = black ? 'B' : 'R';
	if(next != NULL)
		next->alternating(!black);
	return this;
}

//Add new card to list
void Card::operator+=(Card *next) {
	if(this->next == NULL)
		this->next = next;
	else
		*(this->next) += next;
}

//Print full stack of cards
void Card::printStack() {
	std::cout << "{" << cardFace(data) << "," << data.suit << "}, ";
	if(next != NULL)
		next->printStack();
}

//Get card face from card data
char cardFace(Solisp::cardData card) {
	switch(card.value) {
		case 1:
			return 'A';
		case 10:
			return 'T';
		case 11:
			return 'J';
		case 12:
			return 'Q';
		case 13:
			return 'K';
	}
	return '0' + card.value;
}