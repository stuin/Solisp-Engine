#include "../Interpreter/card.h"

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and additional functions
 */

//Added type lists
#define type_count 6
enum cell_type {EXPR, STRING, NUMBER, LIST, CARD, FILTER};
cell_type type_conversions[type_count][type_count] = {
	{NUMBER, STRING, LIST, CARD, FILTER, EXPR},
	{STRING, NUMBER, LIST, CARD, EXPR},
	{NUMBER, STRING, CARD, EXPR},
	{LIST, FILTER, NUMBER, STRING, CARD, EXPR}
};

//Import lisp system
#define addons true
#include "lisp.h"

string str_eval_cont(cell const &c) {
	//Card is stored as string
	if(type == CARD)
		return std::get<string>(c.content); 
}

int num_eval_cont(cell const &c) {
	if(type == CARD) {
		string s = std::get<string>(c.content);
		return std::stoi(s.substr(0, s.length() - 1));
	}
}

sexpr list_eval_cont(cell const &c) {
	if(c.type == FILTER)
		return std::get<sexpr>(c.content);
}

cardData to_card(string s) {
	cardData data;
	data.value = std::stoi(s.substr(0, s.length() - 1));
	data.face = s.back();
	return data;
}

cardData card_eval(cell const &c) {
	if(c.type == CARD)
		return to_card(std::get<string>(c.content));
	if(c.type == STRING) {
		string s = std::get<string>(c.content);
		if(s.length() >= 2 && s.length() <= 3)
			return to_card(std::get<string>(c.content));
	}
	if(c.type == NUMBER)
		return { 'N',  (char)std::get<int>(c.content) };
}