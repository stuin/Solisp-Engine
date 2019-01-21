#include "functions.h"

/*
 * Created by Stuart Irwin on 7/1/2019.
 * Deck building functions.
 */

//Add single card to deck
Deck *addCard(Deck **list, int value) {
	Deck *card = (Deck *) malloc(sizeof Deck)

	if(card != NULL) {
		card->face = value;
		card->suit = 'N';
		card->next = *list;

		*list = card;
	}

	return card;
}

//Add copy of card to list
Deck *cloneCard(Deck *source, Deck **list) {
	Deck *card = (Deck *) malloc(sizeof Deck)

	if(card != NULL) {
		card->face = source->face;
		card->suit = source->suit;
		card->next = *list;

		*list = card;
	}

	return card;
}

//Check if card is contained by list
int containsCard(Deck *list, Deck *card) {
	while(list != NULL) {
		if(list->face == card->face && matchSuit(list->suit, card->suit)) {
			return 1;
		}
		list = list->next;
	}
	return 0;
}

//Append list of cards together
Deck *addDeck(Deck *first, Deck *second) {
	Deck **current = &first;
	while(*current != NULL) {
		current = &(current->next);
	}

	*current = second;
	return first;
}

//Make a complete copy of a card list
Deck *cloneDeck(Deck *source) {
	Deck *list = NULL;

	while(source != NULL) {
		cloneCard(source, list);
		source = source->next;
	}
}

//Build standard deck list
Deck *buildDeck(int start, int end) {
	Deck *card = NULL;

	for(int i = start; i <= end; i++) {
		addDeck(&card, i);
	}

	return card;
}

//Remove cards from list
Deck *removeDeck(Deck **list, Deck *cards) {
	if(*list == NULL || cards = NULL) {
		return;
	}

	if(containsDeck(cards, *list)) {
		*list = *list->next;
		removeDeck(list, cards);
	} else {
		removeDeck(&(list->next, cards));
	}

	return list;
}

//Set suit for every card in list
Deck *setSuit(Deck *list, char suit) {
	while(list != NULL) {
		list->suit = suit;
		list = list->next;
	}

	return list;
}

//Duplicate Deck for each suit
Deck *fourSuit(Deck *list) {
	suit(list, 'D');
	Deck *hearts = suit(cloneDeck(list), 'H');
	Deck *spades = suit(cloneDeck(list), 'S');
	Deck *clubs = suit(cloneDeck(list), 'C');

	appendDeck(list, appendDeck(hearts, appendDeck(spades, clubs)));
	return list;
}

//Check if suits are a valid match
int matchSuit(char a, char b) {
	//Null suit or direct match
	if(a == 'N' || b == 'N' || a == b) {
		return 1;
	}

	//Color match
	if((a == 'B' || b == 'B') && (a == 'S' || b == 'S' || a == 'C' || b == 'C')) {
		return 1;
	}
	if((a == 'R' || b == 'R') && (a == 'H' || b == 'H' || a == 'D' || b == 'D')) {
		return 1;
	}

	return 0;
}