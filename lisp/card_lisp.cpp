#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 20/12/2019.
 * Actual solitaire conversions
 */

//Convert stored string to card
cardData CardEnviroment::to_card(string s) {
	cardData data;
	data.value = std::stoi(s.substr(1));
	data.suit = s.front();
	return data;
}

//Convert card to string
string CardEnviroment::to_string(cardData card) {
	return string(1, card.suit) + std::to_string(card.value);
}

//Convert special types to strings
string CardEnviroment::str_eval_cont(cell const &c, bool literal) {
	//Card is stored as string
	switch(c.type) {
		case CARD:
			return std::get<string>(c.content);
		case DECK: case FILTER: case TAGFILTER: case LAYOUT:
			//Treat as normal list
			string output;
			sexpr vec = std::get<sexpr>(c.content);
			for(cell s : vec)
				output += str_eval(s, literal) + " ";
			return output;
	}

	CONVERTERROR("string");
}

//Convert special types to numbers
int CardEnviroment::num_eval_cont(cell const &c) {
	if(c.type == CARD)
		return card_eval(c).value;

	CONVERTERROR("number");
}

//Convert special types to lists
sexpr CardEnviroment::list_eval_cont(cell const &c) {
	if(c.type == DECK || c.type == FILTER || c.type == TAGFILTER)
		return std::get<sexpr>(c.content);

	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
}

//Convert to card
cardData CardEnviroment::card_eval(cell const &c) {
	sexpr list;
	switch(c.type) {
		case EXPR:
			return card_eval(eval(c));
		case NUMBER: case CHAR:
			return to_card("N" + std::to_string(std::get<int>(c.content)));
		case DECK:
			list = std::get<sexpr>(c.content);
			if(list.size() > 1)
				throw std::domain_error("Deck with multiple items to single card.");
			return card_eval(list[0]);
		case STRING: case CARD:
			string s = std::get<string>(c.content);

			//Try base conversion
			if(s.length() >= 2 && s.length() <= 3)
				return to_card(std::get<string>(c.content));

			//Check if variable
			cell *var = get(s);
			if(var != NULL)
				return card_eval(*var);

			throw std::domain_error("Cannot convert to card from string " + s);
	}

	CONVERTERROR("card");
}

//Convert cell to deck
sexpr CardEnviroment::deck_eval(cell const &c) {
	sexpr array;
	sexpr output;
	string s;
	switch(c.type) {
		case EXPR:
			return deck_eval(eval(c));
		case DECK:
			return std::get<sexpr>(c.content);
		case TAGFILTER:
			return deck_eval(cell(filter_eval(c), FILTER));
		case LIST:
			array = std::get<sexpr>(c.content);

			//Convert all contents to cards
			for(cell value : array)
				output.push_back(force_eval[CARD](this, value));
			return output;
		case STRING: case CARD:
			//Try base conversion
			s = std::get<string>(c.content);
			if(s.length() >= 1 && s.length() <= 3) {
				output.push_back(force_eval[CARD](this, s));
				return output;
			}
			break;
		case FILTER:
			array = std::get<sexpr>(c.content);

			//Flatten contained decks into one
			for(cell value : array) {
				sexpr deck = deck_eval(value);
				output.insert(output.end(), deck.begin(), deck.end());
			}
			return output;
	}
	return list_eval(c);
}

//Convert cell to filter
sexpr CardEnviroment::filter_eval(cell const &c) {
	sexpr array;
	sexpr output;
	switch(c.type) {
		case EXPR:
			return filter_eval(eval(c));
		case FILTER:
			return std::get<sexpr>(c.content);
		case TAGFILTER:
			return filter_eval(std::get<sexpr>(c.content)[0]);
		case DECK:
			output.push_back(c);
			return output;
		case LIST:
			array = std::get<sexpr>(c.content);

			//Convert all contents to decks
			for(cell value : array)
				output.push_back(force_eval[DECK](this, value));
			return output;
	}
	return list_eval(c);
}

//Convert cell to filter with tag
sexpr CardEnviroment::tagfilter_eval(cell const &c, filter_type open) {
	switch(c.type) {
		case EXPR: case STRING:
			return tagfilter_eval(eval(c), open);
		case LIST: case TAGFILTER:
			return std::get<sexpr>(c.content);
	}

	//Add tag to filter
	sexpr *output = new sexpr();
	output->push_back(force_eval[FILTER](this, c));
	output->push_back(open);
	return *output;
}

//Convert cell to layout
sexpr CardEnviroment::layout_eval(cell const &c) {
	switch(c.type) {
		case EXPR:
			return layout_eval(eval(c));
		case LAYOUT:
			return std::get<sexpr>(c.content);
	}

	CONVERTERROR("layout");
}

