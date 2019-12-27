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
	return card.suit + std::to_string(card.value);
}

//Build a function to set the suit of a list of cards
builtin CardEnviroment::setSuits(char suit) {
	return [suit](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		sexpr array = ((CardEnviroment*)env)->deck_eval(*pos++);

		//Add all cards to new deck
		for(cell c : array) {
			cardData d = ((CardEnviroment*)env)->card_eval(c);
			d.suit = suit;
			output->push_back(d);
		}

		DONE;
		return cell(*output, DECK);
	};
}

builtin CardEnviroment::buildLayout(layout_type index) {
	return [index](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(index);
		while(pos != end)
			output->push_back(*pos++);
		return cell(*output, LAYOUT);
	};
}

//Convert special types to strings
string Enviroment::str_eval_cont(cell const &c, bool literal) {
	//Card is stored as string
	if(c.type == CARD)
		return ((CardEnviroment*)this)->to_string(std::get<cardData>(c.content));
	if(c.type == DECK || c.type == FILTER || c.type == LAYOUT) {
		//Treat as normal list
		string output;
		sexpr vec = std::get<sexpr>(c.content);
		for(cell s : vec)
			output += str_eval(s, literal) + " ";
		return output;
	}
	throw std::domain_error("Cannot convert to string from type " + std::to_string(c.type));
}

//Convert special types to numbers
int Enviroment::num_eval_cont(cell const &c) {
	if(c.type == CARD)
		return std::get<cardData>(c.content).value;
	throw std::domain_error("Cannot convert to number from type " + std::to_string(c.type));
}

//Convert special types to lists
sexpr Enviroment::list_eval_cont(cell const &c) {
	if(c.type == DECK || c.type == FILTER)
		return std::get<sexpr>(c.content);

	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
}

//Convert to card
cardData CardEnviroment::card_eval(cell const &c) {
	if(c.type == CARD)
		return std::get<cardData>(c.content);
	if(c.type == STRING) {
		string s = std::get<string>(c.content);

		//Try base conversion
		if(s.length() >= 2 && s.length() <= 3)
			return to_card(std::get<string>(c.content));

		//Try locating variable
		auto it = vars.find(s);
		if(it != vars.end())
			return card_eval(it->second);

		throw std::domain_error("Cannot convert to card from string " + s);
	}
	if(c.type == NUMBER)
		return to_card("N" + std::to_string(std::get<int>(c.content)));
	if(c.type == STRING) {
		//Try locating variable
		auto it = vars.find(std::get<string>(c.content));
		if(it != vars.end())
			return card_eval(it->second);
	}
	if(c.type == DECK) {
		sexpr list = std::get<sexpr>(c.content);
		if(list.size() > 1)
			throw std::domain_error("Deck with multiple items to single card.");
		return card_eval(list[0]);
	}
	if(c.type == EXPR)
		return card_eval(eval(c, CARD));

	throw std::domain_error("Cannot convert to card from type " + std::to_string(c.type));
}

//Convert cell to deck
sexpr CardEnviroment::deck_eval(cell const &c) {
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
	if(c.type == STRING) {
		//Try locating variable
		auto it = vars.find(std::get<string>(c.content));
		if(it != vars.end())
			return deck_eval(it->second);
	}
	if(c.type == EXPR)
		return deck_eval(eval(c, DECK));
	return list_eval(c);
}

//Convert cell to filter
sexpr CardEnviroment::filter_eval(cell const &c) {
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
	if(c.type == STRING) {
		//Try locating variable
		auto it = vars.find(std::get<string>(c.content));
		if(it != vars.end())
			return filter_eval(it->second);
	}
	if(c.type == EXPR)
		return filter_eval(eval(c, FILTER));
	return list_eval(c);
}

//Convert cell to layout
sexpr CardEnviroment::layout_eval(cell const &c) {
	if(c.type == LAYOUT || c.type == LIST)
		return std::get<sexpr>(c.content);
	if(c.type == STRING) {
		//Try locating variable
		auto it = vars.find(std::get<string>(c.content));
		if(it != vars.end())
			return layout_eval(it->second);
	}
	if(c.type == EXPR)
		return layout_eval(eval(c, LAYOUT));
	return list_eval(c);
}

