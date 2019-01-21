/*
 *	Created by Stuart Irwin on 7/1/2019.
 * 	Data structures for the solisp system.
 */

//Struct for a card set
typedef struct card {
	int face;
	char suit;
	struct card *next;
} Deck;

//Struct for a pattern set
typedef struct pattern {
	Deck *deck;
	struct pattern *next;
} Pattern;

//Struct for a feature set
typedef struct feature {
	char type[15];
	int value1;
	int value2;
	struct feature *next;
} Feature;

//Struct for the layout/slot tree
typedef struct layout {
	struct layout *inside;
	struct layout *next;
	Feature *first;
} Layout;
