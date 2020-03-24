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

//Add card to list in random location
Card *Card::shuffle(Card *next) {
	if(index == 0)
		index = -2;
	else
		index--;

	//Get random space in list
	int i = rand() % -index;
	//std::cout << "Shuffling, index = " << -index << ", i = " << i << "\n";

	//Add card to linked list
	if(i == 0) {
		//Replace first card
		next->set_next(this);
		next->index = index;
		index = 0;
		return next;
	} else {
		//Reach selected spot
		Card *pos = this;
		while(i > 1) {
			pos = pos->next;
			i--;
		}

		//Insert card into space
		next->set_next(pos->next);
		pos->set_next(next);
		return this;
	}
}

//Add new card to list
void Card::operator+=(Card *next) {
	if(this->next == NULL)
		this->next = next;
	else
		*(this->next) += next;
}

//Print full stack of cards
string Card::print_stack() {
	string s = "";
	if(next != NULL)
		s = next->print_stack();
	s += "{" + string(1, card_face(data)) + "," + string(1, data.suit) + "}, ";
	return s;
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