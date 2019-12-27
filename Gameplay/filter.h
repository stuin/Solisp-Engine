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
	Filter(bool open) {
		this->content = NULL;
		this->open = open;
	}

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
		while(filter != NULL && !filter->matches(current->get_data())) {
			filter = filter->get_next();
		}

		//Move through new cards alongside filter
		while(current != NULL && filter != NULL && filter->matches(current->get_data())) {
			current = current->get_next();
			filter = filter->get_next();
			count--;

			//Mark end of new stack
			if(count == 0)
				current = NULL;
		}

		//If old stack is empty, check if end of filter
		if(current == NULL && oldCard == NULL && (open || filter == NULL))
			return true;

		//Check if new card lines up with old card
		if(current == NULL && filter != NULL && filter->matches(oldCard->get_data()))
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
		if(content == NULL)
			content = card;
		else
			*this += new Filter(card, open);
	}

	//Flatten all filters into one deck
	Card *flatten(bool deleteSafe=true) {
		Card *output = content->clone();
		if(next != NULL)
			*output += next->flatten(deleteSafe);
		return output;
	}
};
