#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 20/12/2019.
 * Lisp type convertions and definitions
 */

//Base eval function
cell Enviroment::eval(cell const &c, int type) {
	if(c.type == EXPR)
		return std::visit([type, this](auto const &c) { return this->eval(c, type); }, c.content);
    return c;
}

//Convert to string
string Enviroment::str_eval(cell const &c, bool literal) {
	if(c.type == EXPR && !literal)
		return str_eval(eval(c, STRING));
	switch(c.type) {
		case STRING:
			return std::get<string>(c.content);
		case NUMBER:
			return std::to_string(std::get<int>(c.content));
		case CHAR:
			return string(1, std::get<int>(c.content));
		case LIST: case EXPR:
			string output;
			sexpr vec = std::get<sexpr>(c.content);
			for(cell s : vec)
				output += str_eval(s, literal) + " ";
			return output;
	}

	return str_eval_cont(c, literal);
}

string Enviroment::str_eval_cont(cell const &c, bool literal) {
	throw std::domain_error("Cannot convert to string from type " + std::to_string(c.type));
}

//Convert to number
int Enviroment::num_eval(cell const &c) {
	switch(c.type) {
		case EXPR:
			return num_eval(eval(c, NUMBER));
		case NUMBER: case CHAR:
			return std::get<int>(c.content);
		case STRING:
			string s = std::get<string>(c.content);

			//If numerical string
			if(isdigit(s[0]) || s[0] == '-')
				return std::stoi(s);

			//Try accessing variable value
			auto it = vars.find(s);
			if(it != vars.end())
				return num_eval(it->second);

			//Try boolean values
			if(s[0] == 't' || s[0] == 'T')
				return 1;
			if(s[0] == 'f' || s[0] == 'F')
				return 0;

			throw std::domain_error("No variable or value found for " + s);
	}

	return num_eval_cont(c);
}

int Enviroment::num_eval_cont(cell const &c) {
	throw std::domain_error("Cannot convert to number from type " + std::to_string(c.type));
}

//Convert to char
char Enviroment::char_eval(cell const &c) {
	switch(c.type) {
		case EXPR:
			return char_eval(eval(c, CHAR));
		case NUMBER: case CHAR:
			return std::get<int>(c.content);
		case STRING:
			string s = std::get<string>(c.content);
			if(s.length() == 1)
				return s[0];
			throw std::domain_error("Cannot convert to char from multi-char string");
	}

	return char_eval_cont(c);
}

char Enviroment::char_eval_cont(cell const &c) {
	throw std::domain_error("Cannot convert to char from type " + std::to_string(c.type));
}

//Convert to list
sexpr Enviroment::list_eval(cell const &c) {
	switch(c.type) {
		case EXPR:
			return list_eval(eval(c, LIST));
		case LIST:
			return std::get<sexpr>(c.content);
		case STRING: case CHAR:
			//Try accessing variable value
			string s = str_eval(c);
			auto it = vars.find(s);
			if(it != vars.end())
				return list_eval(it->second);

			//Convert string to char list
			sexpr output;
			for(char a : s)
				output.push_back(cell(a));
			return output;
	}

	return list_eval_cont(c);
}

sexpr Enviroment::list_eval_cont(cell const &c) {
	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
}

//Actual expression evaluation
cell Enviroment::eval(sexpr const &s, int type) {
	if(s.begin() == s.end())
		throw std::domain_error("Empty sexpr");

	//Search library for function
	string name = str_eval(*s.begin());
	auto b = library.find(name);
	if(b != library.end())
		return b->second(this, ++s.begin(), s.end());

	throw std::invalid_argument{name + " not in the library"};
}

//Build a function to force set the type of a structure
builtin Enviroment::forcer(int type) {
	return [type](Enviroment* env, marker pos, marker end) {
		cell output = env->force_eval[type](env, *pos++);
		DONE;
		return output;
	};
}