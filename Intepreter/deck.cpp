#include "structures.h"

/*
 * Created by Stuart Irwin on 18/1/2019.
 * Deck building functions.
 */

//Create single card node
DeckNode::DeckNode(int value) {
	face = value;
}

//Copy deck node values
void DeckNode::copy(const DeckNode& source) {
	face = source.face;
	suit = source.suit;
}

//Compare single card values
bool DeckNode::operator==(const DeckNode& card) {
	if(!(suit == card.suit)) {
		return false;
	} else if(face == card.face || face == 'N' || card.face == 'N') {
		return true;
	}

	return false;
}

//Create single card list
Deck::Deck(int value) {
	*this += new DeckNode(value);
}

//Create stack of cards
Deck::Deck(int start, int end) {
	Deck(start);

	//Create card for every number
	for(int i = start + 1; i < end; i++) {
		*this += new DeckNode(i);
	}
}

//Set suit for all cards
void Deck::setSuit(char suit) {
	DeckNode *node = (DeckNode) head;

	//Loop through deck
	while(node != NULL) {
		node->suit = suit;
		node = (DeckNode) node->next;
	}
}

//Create copy of deck for each suit
void Deck::fourSuit() {
	//Create clone decks
	Deck clubs = *this;
	Deck hearts = *this;
	Deck spades = *this;

	//Set deck suits
	setSuit(diamonds);
	clubs.setSuit(clubs);
	hearts.setSuit(hearts);
	spades.setSuit(spades);

	//Add decks together
	*this += clubs;
	*this += hearts;
	*this += spades;
}

//Reverse whole order of list
void Deck::reverse() {
	ListNode *prev = head;
	ListNode *node;

	//Check first node
	if(prev != NULL) {
		node = prev->next;
		prev->next = NULL;
	}

	//Loop over list
	while(prev != NULL && node != NULL) {
		ListNode *next = node->next;

		//Make list changes
		node->next = prev;
		prev = node;
		node = next;
	}

	//Finish up list
	head = prev;
}

//Copy first card to end of list
void Deck::loop() {
	ListNode *node = head;

	//Locate first card
	while(node != NULL) {
		node = node->next;
	}

	//Add on copy of card
	DeckNode next = *node;
	*this += next;
}

//Duplicate deck and attach to self
void Deck::operator*=(const int& count) {
	Deck source = *this;

	//Clone original deck and append to deck
	for(int i = 0; i < count; i++) {
		Deck add = source;
		*this += add;
	}

	//Delete copy of original
	source.head->remove();
}

//Add single card to list
void Deck::operator+=(const int& value) {
	*this += new DeckNode(value);
}

//Remove list of cards from deck
void Deck::operator-=(const Deck& deck) {
	ListNode *node = deck.head;

	//Loop through removed deck
	while(node != NULL) {
		*this -= node;
	}
}

//Remove single card from deck
void Deck::operator-=(const DeckNode& card) {
	ListNode **node = &head;

	//Loop through current deck
	while(*node != NULL) {
		if(*node == card) {
			//Remove card from deck
			ListNode removed = *node;
			*node = *node->next;
			delete removed;
		}

		//Next card
		node = &(*node->next);
	}
}

//Check if suits are a valid match
bool operator==(const CardSuit& a, const CardSuit& b) {
	//Null suit or direct match
	if(a == null || b == null || a == b) {
		return true;
	}

	//Color match
	if((a == black || b == black) && (a == spades || b == spades || a == clubs || b == clubs)) {
		return true;
	}
	if((a == red || b == red) && (a == hearts || b == hearts || a == diamonds || b == diamonds)) {
		return true;
	}

	return false;
}