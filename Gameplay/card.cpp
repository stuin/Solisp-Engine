#include "filter.h"

#include <iostream>

/*
 * Created by Stuart Irwin on 5/10/2019.
 * Card setup functions
 */

using Solisp::Card;
using Solisp::Filter;

//Constructor for full list of cards
Card::Card(std::vector<char> values, int i) {
	data.value = values[i];
	data.suit = values[i + 1];
	hidden = false;

	if(i > 0)
		next = new Card(values, i - 2);
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
	return s + print_card(data) + ", ";
}

//Print single card
string print_card(Solisp::cardData card) {
	return "{" + string(1, card_face(card)) + "," + string(1, card.suit) + "}";
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