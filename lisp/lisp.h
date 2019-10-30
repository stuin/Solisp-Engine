#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <stdexcept>
#include <iostream>

/*
 * Created by Stuart Irwin on 28/10/2019.
 * Based on https://kirit.com/Build%20me%20a%20LISP and https://gist.github.com/ofan/721464
 */

struct cell;
enum cell_type {EXPR, STRING, NUMBER, LIST};

using std::string;
using sexpr = std::vector<cell>;
using marker = sexpr::const_iterator;
using builtin = std::function<auto(marker, marker)->cell>;

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int> content;

	//Constructors
	cell() { cell(""); }
	cell(string s) : content{std::move(s)} { type = STRING; }
	cell(int n) : content{std::move(n)} { type = NUMBER; }
	cell(sexpr s) : content{std::move(s)} { type = EXPR; }
	cell(sexpr s, cell_type t) : content{std::move(s)} { type = t; }
};

//Variable storage
std::map<string, cell> env;

//Library structure
std::map<string, builtin> library[4];
void build_library();

//Retrieving functions from library
builtin check_shelf(string name, std::vector<cell_type> type) {
	int i = 0;
	do {
		auto b = library[type[i]].find(name);
		if(b != library[type[i]].end())
			return b->second;
		i++;
	} while(type[i - 1] != EXPR);
	throw std::invalid_argument{name + " not in the library"};
}
builtin search_library(string name, cell_type type) {
	switch(type) {
		case EXPR:
			return check_shelf(name, {STRING, NUMBER, LIST, EXPR});
		case STRING: 
			return check_shelf(name, {STRING, NUMBER, EXPR});
		case NUMBER:
			return check_shelf(name, {NUMBER, STRING, EXPR});
		case LIST:
			return check_shelf(name, {LIST, EXPR});
	}
	throw std::invalid_argument{name + " not in the library"};
}
 
//Base eval function
cell eval(sexpr const &c, cell_type type);
cell eval(cell const &c, cell_type type) {
	if(c.type == EXPR)
		return std::visit([type](auto const &c) { return eval(c, type); }, c.content);
    return c;
}

//Convert to string
string str_eval(cell const &c) {
	if(c.type == STRING)
		return std::get<string>(c.content);
	if(c.type == NUMBER)
		return std::to_string(std::get<int>(c.content));
	if(c.type == EXPR)
		return str_eval(eval(c, STRING));
	throw std::domain_error("Cannot convert to string");
}

//Convert to number
int num_eval(cell const &c) { 
	if(c.type == NUMBER)
		return std::get<int>(c.content);
	if(c.type == STRING) {
		string s = std::get<string>(c.content);

		//If numerical string
		if(isdigit(s[0]) || s[0] == '-')
			return std::stoi(s);

		//Try accessing variable value
		auto it = env.find(s);
		if(it != env.end())
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
	throw std::domain_error("Cannot convert to number");
}

//Convert to list
sexpr list_eval(cell const &c) {
	if(c.type == LIST)
		return std::get<sexpr>(c.content);
	if(c.type == STRING) {
		//Try accessing variable value
		string s = str_eval(c);
		auto it = env.find(s);
		if(it != env.end())
			return list_eval(it->second);
		throw std::domain_error("No variable found for " + s);
	}
	if(c.type == EXPR)
		return list_eval(eval(c, LIST));
	throw std::domain_error("Cannot convert to list");
}

//Actual expression evaluation
cell eval(sexpr const &s, cell_type type) {
	if(s.begin() == s.end())
		throw std::domain_error("Empty sexpr");
	else {
		string name = str_eval(*s.begin());
		return search_library(name, type)(++s.begin(), s.end());
	}
}

//Test main function
int main() {
	try {
		using namespace std::string_literals;

		//Build test tree
		auto const prog = sexpr{
			"+"s,
			sexpr{"Set"s, "x"s, 14},
			" "s,
			sexpr{"If"s,
				sexpr{"=="s, 13, "x"s},
				" True "s,
				sexpr{"For-Each"s, 
					sexpr{"List"s, 2, 3, 4},
					"i"s,
					sexpr{"-"s, "i"s, 1}
				}
			},
			" Hello "s, "world!"s
		};

		//Build library
		build_library();
		
		//Run test code
		std::cout << str_eval(prog) << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}