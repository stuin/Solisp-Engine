#include "../Interpreter/card.h"

#include <vector>
#include <string>
#include <variant>

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and convertions
 */

//Required early type definitions
struct cell;
using std::string;
using sexpr = std::vector<cell>;
using card = Solisp::cardData;

//Expanded type lists
#define type_count 7
enum cell_type {EXPR, STRING, NUMBER, LIST, CARD, DECK, FILTER};
cell_type type_conversions[type_count][type_count] = {
	{NUMBER, STRING, LIST, CARD, DECK, FILTER, EXPR},
	{STRING, NUMBER, LIST, CARD, DECK, FILTER, EXPR},
	{NUMBER, STRING, CARD, EXPR},
	{LIST, DECK, FILTER, NUMBER, STRING, CARD, EXPR},
	{CARD, NUMBER, STRING, EXPR},
	{DECK, FILTER, LIST, CARD, NUMBER, STRING, EXPR},
	{FILTER, DECK, LIST, CARD, NUMBER, STRING, EXPR}
};

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int, card> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, cell_type t = STRING) : content{std::move(s)} { type = t; }
	cell(int n, cell_type t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, cell_type t = EXPR) : content{std::move(s)} { type = t; }
	cell(card c, cell_type t = CARD) : content{std::move(c)} { type = t; }

	//Equality
	friend bool operator==(const cell &first, const cell &second) {
		return first.content == second.content;
	}
};

//Import main lisp system
#define addons true
#include "library.h"

//Convert stored string to card
card to_card(string s) {
	card data;
	data.value = std::stoi(s.substr(1));
	data.suit = s.front();
	return data;
}

//Convert card to string
string to_string(card card) {
	return card.suit + std::to_string(card.value);
}

//Convert special types to strings
string str_eval_cont(cell const &c) {
	//Card is stored as string
	if(c.type == CARD)
		return to_string(std::get<card>(c.content));
	if(c.type == DECK || c.type == FILTER) {
		//Treat as normal list
		string output;
		sexpr vec = std::get<sexpr>(c.content);
		for(cell s : vec)
			output += str_eval(s) + " ";
		return output;
	}
	throw std::domain_error("Cannot convert to string from type " + std::to_string(c.type));
}

//Convert special types to numbers
int num_eval_cont(cell const &c) {
	if(c.type == CARD)
		return std::get<card>(c.content).value;
	throw std::domain_error("Cannot convert to number from type " + std::to_string(c.type));
}

//Convert special types to lists
sexpr list_eval_cont(cell const &c) {
	if(c.type == DECK || c.type == FILTER)
		return std::get<sexpr>(c.content);

	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
}

//Convert to card
card card_eval(cell const &c) {
	if(c.type == CARD)
		return std::get<card>(c.content);
	if(c.type == STRING) {
		string s = std::get<string>(c.content);

		//Try base conversion
		if(s.length() >= 2 && s.length() <= 3)
			return to_card(std::get<string>(c.content));

		//Try locating variable
		auto it = env.find(s);
		if(it != env.end())
			return card_eval(it->second);
	}
	if(c.type == NUMBER)
		return to_card("N" + std::to_string(std::get<int>(c.content)));
	if(c.type == EXPR)
		return card_eval(eval(c, CARD));

	throw std::domain_error("Cannot convert to card from type " + std::to_string(c.type));
}

//Convert cell to deck
sexpr deck_eval(cell const &c) {
	if(c.type == DECK)
		return std::get<sexpr>(c.content);
	if(c.type == LIST) {
		sexpr array = std::get<sexpr>(c.content);
		sexpr *output = new sexpr();

		//Convert all contents to cards
		for(cell value : array)
			output->push_back(cell(card_eval(value), CARD));
		return *output;
	}
	if(c.type == FILTER) {
		sexpr array = std::get<sexpr>(c.content);
		sexpr *output = new sexpr();

		//Flatten contained decks into one
		for(cell value : array) {
			sexpr deck = deck_eval(value);
			output->insert(output->end(), deck.begin(), deck.end());
		}
		return *output;
	}
	if(c.type == EXPR)
		return deck_eval(eval(c, DECK));
	return list_eval(c);
}

//Convert cell to filter
sexpr filter_eval(cell const &c) {
	if(c.type == FILTER)
		return std::get<sexpr>(c.content);
	if(c.type == LIST) {
		sexpr array = std::get<sexpr>(c.content);
		sexpr *output = new sexpr();

		//Convert all contents to decks
		for(cell value : array)
			output->push_back(cell(deck_eval(value), DECK));
		return *output;
	}
	if(c.type == DECK) {
		sexpr *output = new sexpr();
		output->push_back(c);
		return *output;
	}
	if(c.type == EXPR)
		return filter_eval(eval(c, FILTER));
	return list_eval(c);
}