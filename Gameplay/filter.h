#pragma once

#include "card.h"

#include <iostream>

using std::cout;

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Filter on card type
 */

class Solisp::Filter {
private:
	Card *content;
	Filter *next = NULL;
	filter_type open;

public:
	Filter(filter_type open) {
		this->content = NULL;
		this->open = open;
	}

	Filter(Card *card, filter_type open=CLOSED) {
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

		if(current != NULL) {
			if(open != ALL) {
				//Check if top card matches any card in filter
				while(filter != NULL && !filter->matches(current->get_data())) {
					filter = filter->get_next();
				}
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
		}

		if(current == NULL) {
			//If old stack is empty, check if end of filter
			if(oldCard == NULL && (open == OPEN || filter == NULL))
				return true;

			//Check if new card lines up with old card
			if(filter != NULL && oldCard != NULL && filter->matches(oldCard->get_data()))
				return true;
		}

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
