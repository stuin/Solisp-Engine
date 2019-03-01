#include "enums.h"

/*
 * Created by Stuart Irwin on 7/1/2019.
 * Data structures for the solisp system.
 */

//Basic list object
class ListNode {
	ListNode *next = NULL;

	//Node constructor
	ListNode(const ListNode& source);

	//Node functions
	void copy(const ListNode& source);
	void remove();
}

//Basic list head
class List {
	ListNode *head = NULL;

	//List constructors
	List(const List& source);

	//List operators
	void operator+=(const List& list);
	void operator+=(const ListNode& node);
}

//Single card object
class DeckNode : ListNode {
	CardFace face;
	CardSuit suit = null;

	//DeckNode constructors
	DeckNode(int value);
	void copy(const DeckNode& source);

	//DeckNode operators
	bool operator==(const DeckNode& card);
}

class Deck : List {
	//Deck constructors
	Deck(int value);
	Deck(int start, int end);

	//Deck functions
	void setSuit(CardSuit suit);
	void fourSuit();
	void reverse();
	void loop();
	Pattern alternatingPattern();
	Pattern fourSuitPattern();

	//Deck operators
	void operator*=(const int& count);
	void operator+=(const int& value);
	void operator-=(const Deck& deck);
	void operator-=(const DeckNode& card);
}

//Basic helper function
bool matchSuit(char a, char b);

//List of multiple decks
class PatternNode : ListNode {
	DeckNode *deck;

	//PatternNode functions
	PatternNode(DeckNode& deck);
}

class Pattern : List {
	//Pattern functions
	Pattern(Deck& deck);
	void operator+=(const Deck& deck);
}

//Base properties to apply to a layout
class FeatureNode : ListNode {
	FeatureType type;

	//FeatureNode functions
	FeatureNode(FeatureType& type);
	void copy(const FeatureNode& source);
}

//Numerical given feature
class NumFeatureNode : FeatureNode {
	int value1;
	int value2;

	//FeatureNode functions
	NumFeatureNode(FeatureType& type, int value1, int value2);
	void copy(const NumFeatureNode& source);
}

//Pattern based feature
class PatternFeatureNode : FeatureNode {
	Pattern *value;

	//FeatureNode functions
	NumFeatureNode(FeatureType& type, Pattern& value);
	void copy(const PatternFeatureNode& source);
}

class Feature : List {
	//Feature functions
	Feature(FeatureNode& node);
}

//Layout slot for cards only
class SlotNode : ListNode {
	SlotType type;
	FeatureNode *features;

	//SlotNode
	SlotNode(SlotType& type, Feature& feature);
}

//LayoutNode for slot arrangement
class LayoutNode : ListNode {
	LayoutType type;
	Layout *inside;

	//LayoutNode functions
	LayoutNode(LayoutType& type);
	void operator+=(const Layout& child);
}

class Layout : List {
	//Layout functions
	Layout(SlotType& type, Feature& feature);
	Layout(LayoutType& type);
	Layout(LayoutType& type, Layout& child);
}
