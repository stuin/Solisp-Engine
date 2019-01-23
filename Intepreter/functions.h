#include "structures.h"

/*
 * Created by Stuart Irwin on 18/1/2019.
 * Function lists for the solisp system.
 */

//----deck.c----

//Single card functions
Deck *addCard(Deck **list, int value)
Deck *cloneCard(Deck *source, Deck **list);
int containsCard(Deck *list, Deck *card);

//Full deck functions
Deck *addDeck(Deck *first, Deck *second);
Deck *cloneDeck(Deck *source);
Deck *buildDeck(int start, int end);
Deck *removeDeck(Deck **list, Deck *cards);

//Deck suit functions
Deck *setSuit(Deck *list, char suit);
Deck *fourSuit(Deck *list);
int matchSuit(char a, char b);

//----pattern.c----

//Pattern functions
Pattern *makePattern(Deck *list);
Pattern *addPattern(Pattern *first, Pattern *second);
Pattern *alternatePattern(Deck *list);
Pattern *suitPattern(Deck *list);
int attachPattern(Pattern *source);

//----feature.c----

//Feature making functions
Feature *makeFeature(char type[15], int value1, int value2);
Feature *addFeature(Feature *first, Feature *second);
Feature *cloneFeature(Feature *source);

//----layout.c----

//Layout functions
Layout *makeLayout(char type, Feature *feature);
Layout *addLayout(Layout *first, Layout *second);
Layout *inLayout(Layout *parent, Layout *child);
