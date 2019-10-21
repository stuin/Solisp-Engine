#pragma once
namespace Solisp {
	class Card;
	struct cardData;
	class Filter;
}

#include <cstddef>

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Individual card data
 */

struct Solisp::cardData {
	char value = 'N';
	char suit = 'N';
};

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
		if(data.value != other.value && data.value != 'N')
			return false;

		if(data.suit == 'N' || data.suit == other.suit)
			return true;

		switch(data.suit) {
			case 'R':
				if(other.suit == 'H' || other.suit == 'D')
					return true;
			case 'B':
				if(other.suit == 'C' || other.suit == 'S')
					return true;
		}
		return false;
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
		if(next == NULL) {
			index = 0;
			return 0;
		}
		index = next->setIndex(count - 1) + 1;
		return index;
	}

	//Card setup functions
	Card(char start, char end, char suit);
	Card *clone();
	Card *reverse(int index=0, Card *last=NULL);
	Card *withSuit(char suit);
	Card *fourSuit();
	Filter *fourSuitFilter();
	Filter *alternating();
	Card *alternating(bool black);
	void operator+=(Card *next);
};