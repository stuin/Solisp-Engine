#pragma once
namespace Solisp {
	class Card;
	class Filter;
}

#include "../Lisp/card.h"
#include <cstddef>
#include <string>
#include <vector>

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Individual card data
 */

using std::string;
using Solisp::cardData;

class Solisp::Card {
private:
	cardData data;
	bool hidden = true;

	Card *next = NULL;
	int slot = 0;
	int index = 0;

public:
	//Constructors
	Card(Card *card) {
		data = card->data;
		next = card->next;
	}
	Card(cardData data, int slot=0, int index=0) {
		this->data = data;
		this->slot = slot;
		this->index = index;
	}
	Card(char value, char suit) {
		data.value = value;
		data.suit = suit;
	}

	//Deconstructor
	~Card() {
		delete next;
	}

	//General getters
	cardData get_data() {
		return data;
	}
	Card *get_next() {
		return next;
	}
	int get_slot() {
		return slot;
	}
	int get_index() {
		if(index < 0)
			index = 0;
		return index;
	}

	//Get animation frame for card
	int get_frame() {
		switch((char)data.suit) {
			case 'D':
				return data.value - 1;
			case 'C':
				return 13 + data.value;
			case 'H':
				return 27 + data.value;
			case 'S':
				return 41 + data.value;
			case 'R':
				return 13;
			case 'B':
				return 27;
			case 'F':
				return 41;
			case 'N':
				return 55;
		}
		return 0;
	}

	//Check what side is up
	bool is_hidden() {
		return hidden;
	}

	//Check if normal card matches this filter card
	bool matches(Card *other) {
		return data == other->get_data();
	}
	bool matches(cardData other) {
		return data == other;
	}

	//Flip card
	void flip() {
		hidden = !hidden;
	}

	//General setters
	void set_next(Card *next) {
		this->next = next;
	}
	void set_slot(int slot) {
		this->slot = slot;
		index = -1;
	}

	//Make sure index value is set properly
	int set_index(int count) {
		if(count == 0)
			return index;
		if(next == NULL)
			return index = 0;
		return index = next->set_index(count - 1) + 1;
	}

	//Card setup functions
	Card(std::vector<char> values, int i);
	Card *clone();
	Card *reverse(int index=0, Card *last=NULL);
	Card *shuffle(Card *next);
	void operator+=(Card *next);
	string print_stack();
};

