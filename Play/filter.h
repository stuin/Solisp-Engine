#include "card.h"

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Filter on card type
 */

class Filter {
private:
	Card *content;
	Filter *next = NULL;
	bool open;

public:
	Filter(Card *card, bool open=false) {
		this->card = card;
		this->open = open;
	}

	void operator+=(const Filter *next) {
		if(this->next == NULL) 
			this->next = next;
		else
			*(this->next) += next;
	}

	void operator+=(const Card *card) {
		*this += new Filter(card, open);
	}

	//Check if new cards can be placed on old card in filter
	bool matches(int count, Card *newCard, Card *oldCard) {
		Card *current = newCard;
		Card *filter = content;

		//Check if top card matches any card in filter
		while(filter != NULL && !filter->matches(current)) {
			filter = filter->next;
		}

		//Move through new cards alongside filter
		while(current != NULL && filter != NULL && filter->matches(current)) {
			current = current->next;
			filter = filter->next;
			count--;

			//Mark end of new stack
			if(count == 0)
				current == NULL;
		}

		//If old stack is empty, check if end of filter
		if(current == NULL && oldCard == NULL && (open || filter == NULL))
			return true;

		//Check if new card lines up with old card
		if(current == NULL && filter != NULL && filter->matches(oldCard))
			return true;

		//If match not found, move to next filter
		if(next != NULL)
			return next->matches(count, newCard, oldCard);
		else 
			return false;
	}
}