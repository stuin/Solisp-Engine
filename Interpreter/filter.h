#pragma once

#include "card.h"

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Filter on card type
 */

class Solisp::Filter {
private:
	Card *content;
	Filter *next = NULL;
	bool open;

public:
	Filter(Card *card, bool open=false) {
		this->content = card;
		this->open = open;
	}

	//Deconstructor
	~Filter() {
		delete next;
		delete content;
	}

	//Check if new cards can be placed on old card in filter
	bool matches(int count, Card *newCard, Card *oldCard) {
		Card *current = newCard;
		Card *filter = content;

		//Check if top card matches any card in filter
		while(filter != NULL && !filter->matches(current->getData())) {
			filter = filter->getNext();
		}

		//Move through new cards alongside filter
		while(current != NULL && filter != NULL && filter->matches(current->getData())) {
			current = current->getNext();
			filter = filter->getNext();
			count--;

			//Mark end of new stack
			if(count == 0)
				current = NULL;
		}

		//If old stack is empty, check if end of filter
		if(current == NULL && oldCard == NULL && (open || filter == NULL))
			return true;

		//Check if new card lines up with old card
		if(current == NULL && filter != NULL && filter->matches(oldCard->getData()))
			return true;

		//If match not found, move to next filter
		if(next != NULL)
			return next->matches(count, newCard, oldCard);
		else 
			return false;
	}

	//Add filter to end of list
	void operator+=(Filter *next) {
		if(this->next == NULL)
			this->next = next;
		else
			*(this->next) += next;
	}
	void operator+=(Card *card) {
		*this += new Filter(card, open);
	}

	//Flatten all filters into one deck
	Card *flatten(bool deleteSafe=true) {
		if(next != NULL)
			*content += next->flatten(deleteSafe);

		//Make safe for deletion
		Card *output = new Card(content);
		if(deleteSafe)
			content = NULL;
		return output;
	}
};
