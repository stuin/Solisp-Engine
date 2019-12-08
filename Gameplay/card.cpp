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
		output->set_next(next->clone());
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

//Add new card to list
void Card::operator+=(Card *next) {
	if(this->next == NULL)
		this->next = next;
	else
		*(this->next) += next;
}

//Print full stack of cards
void Card::print_stack() {
	std::cout << "{" << card_face(data) << "," << data.suit << "}, ";
	if(next != NULL)
		next->print_stack();
}

//Get card face from card data
char card_face(Solisp::cardData card) {
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