#include "structures.h"

/*
 * Created by Stuart Irwin on 1/22/2019.
 * Pattern building functions.
 */

//Create single node for pattern
PatternNode::PatternNode(Deck& deck) {
	this->deck = &(deck->head);
}

//Create pattern with single node
Pattern::Pattern(Deck& deck) {
	*this += new PatternNode(deck);
}

//Add new deck to pattern
void Pattern::operator+=(const Deck& deck) {
	*this += new PatternNode(deck);
}

//Create pattern of alternating colored cards
Pattern Deck::alternatingPattern() {
	Deck deck = *this;
	DeckNode *first = (DeckNode) head;
	DeckNode *second = (DeckNode) deck->head;

	//Loop through cards
	bool color = true;
	while(first != NULL && second != NULL) {
		//Set both colors
		if(color) {
			first->suit = red;
			second->suit = black;
		} else {
			first->suit = black;
			second->suit = red;
		}

		//Get next cards
		first = first->next;
		second = second->next;
		color = !color;
	}

	//Set up actual pattern
	Pattern pattern = new Pattern(*this);
	pattern += deck;
	return pattern;
}

//Create pattern of four identical suits
Pattern Deck::fourSuitPattern() {
	//Create clone decks
	Deck clubs = *this;
	Deck hearts = *this;
	Deck spades = *this;

	//Set deck suits
	setSuit(diamonds);
	clubs.setSuit(clubs);
	hearts.setSuit(hearts);
	spades.setSuit(spades);

	//Add decks together into pattern
	Pattern pattern = new Pattern(*this);
	pattern += clubs;
	pattern += hearts;
	pattern += spades;
	
	return pattern;
}