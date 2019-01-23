#include "functions.h"

/*
 * Created by Stuart Irwin on 1/22/2019.
 * Pattern building functions.
 */

//Create pattern from deck
Pattern *addPattern(Pattern **list, Deck *deck) {
	Pattern *pattern = (Pattern *) malloc(sizeof Pattern);

	//Put deck into new pattern
	if(pattern !=  NULL) {
		pattern->deck = deck;
		pattern->next = *list;

		*list = pattern;
	}

	return pattern;
}

//Append list of patterns together
Pattern *appendPattern(Pattern *first, Pattern *second) {
	Pattern **current = &first;

	//Find end of list
	while(*current != NULL) {
		current = &(current->next);
	}

	*current = second;
	return first;
}

//Create decks of alternating black and red
Pattern *alternatePattern(Deck *first) {
	Deck *second = cloneDeck(first);

	//Save decks into pattern
	Pattern *pattern = addPattern(NULL, first);
	addPattern(&pattern, second);

	//Loop through decks
	int color = 0;
	while(first != NULL && second != NULL) {
		if(color) {
			first->suit = 'R';
			second->suit = 'B';
		} else {
			first->suit = 'B';
			second->suit = 'R';
		}

		first = first->next;
		second = second->next;
	}

	return pattern;
}

//Create decks for each of four suits
Pattern *suitPattern(Deck *list) {

}

//Link pattern to numerical ID for feature
int attachPattern(Pattern *source) {

}

