#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 20/12/2019.
 * Lisp type convertions and definitions
 */

//Retrieving function from library
builtin Enviroment::search_library(string name, cell_type type) {
	int i = 0;
	do {
		//Check every function in a convertable type
		auto b = library[type_conversions[type][i]].find(name);
		if(b != library[type_conversions[type][i]].end())
			return b->second;
		i++;
	} while(type_conversions[type][i - 1] != EXPR);

	throw std::invalid_argument{name + " not in the library for type " + std::to_string(type)};
}
 
//Base eval function
cell Enviroment::eval(cell const &c, cell_type type) {
	if(c.type == EXPR)
		return std::visit([type, this](auto const &c) { return this->eval(c, type); }, c.content);
    return c;
}

//Convert to string
string Enviroment::str_eval(cell const &c, bool literal) {
	if(c.type == STRING)
		return std::get<string>(c.content);
	if(c.type == NUMBER)
		return std::to_string(std::get<int>(c.content));
	if(c.type == EXPR && !literal)
		return str_eval(eval(c, STRING));
	if(c.type == LIST || c.type == EXPR) {
		string output;
		sexpr vec = std::get<sexpr>(c.content);
		for(cell s : vec)
			output += str_eval(s, literal) + " ";
		return output;
	}
	
	if(addons) return str_eval_cont(c, literal);
	throw std::domain_error("Cannot convert to string from type " + std::to_string(c.type));

}

//Convert to number
int Enviroment::num_eval(cell const &c) { 
	if(c.type == NUMBER)
		return std::get<int>(c.content);
	if(c.type == STRING) {
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
	if(c.type == EXPR)
		return num_eval(eval(c, NUMBER));

	if(addons) return num_eval_cont(c);
	throw std::domain_error("Cannot convert to number from type " + std::to_string(c.type));
}

//Convert to list
sexpr Enviroment::list_eval(cell const &c) {
	if(c.type == LIST)
		return std::get<sexpr>(c.content);
	if(c.type == STRING) {
		//Try accessing variable value
		string s = str_eval(c);
		auto it = vars.find(s);
		if(it != vars.end())
			return list_eval(it->second);
	}
	if(c.type == EXPR)
		return list_eval(eval(c, LIST));
	
	if(addons) return list_eval_cont(c);

	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
}

//Actual expression evaluation
cell Enviroment::eval(sexpr const &s, cell_type type) {
	if(s.begin() == s.end())
		throw std::domain_error("Empty sexpr");
	else {
		string name = str_eval(*s.begin());
		return search_library(name, type)(this, ++s.begin(), s.end());
	}
}

//Build a function to force set the type of a structure
builtin Enviroment::forcer(cell_type type) {
	return [type](Enviroment* env, marker pos, marker end) {
		cell output = env->force_eval[type](env, *pos++);
		DONE;
		return output;
	};
}

void Enviroment::merge_convertors(std::initializer_list<std::initializer_list<cell_type>> added) {
	auto list = added.begin();
	int y = 0;
	while(list != added.end()) {
		auto type = list->begin();
		int x = 0;
		while(type != list->end()) {
			type_conversions[y][x] = *type;
			x++;
			type++;
		}
		list++;
		y++;
	}
}