#pragma once
namespace Solisp {
	class Card;
	class Filter;
}

#include "../lisp/card.h"
#include <cstddef>
#include <string>

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Individual card data
 */

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
	Card(cardData data) {
		this->data = data;
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
	cardData getData() {
		return data;
	}
	Card *getNext() {
		return next;
	}
	int getSlot() {
		return slot;
	}
	int getIndex() {
		return index;
	}

	//Check what side is up
	bool isHidden() {
		return hidden;
	}

	//Check if normal card matches this filter card
	bool matches(cardData other) {
		return data == other;
	}

	//Flip card
	void flip() {
		hidden = !hidden;
	}

	//General setters
	void setNext(Card *next) {
		this->next = next;
	}
	void setSlot(int slot) {
		this->slot = slot;
		index = -1;
	}

	//Make sure index value is set properly
	int setIndex(int count) {
		if(count == 0)
			return index;
		if(next == NULL)
			return index = 0;
		return index = next->setIndex(count - 1) + 1;
	}

	//Card setup functions
	Card(char start, char end, char suit);
	Card *clone();
	Card *reverse(int index=0, Card *last=NULL);
	Card *withSuit(char suit);
	Filter *fourSuit();
	Filter *alternating();
	Card *alternating(bool black);
	void operator+=(Card *next);
	void printStack();
};

