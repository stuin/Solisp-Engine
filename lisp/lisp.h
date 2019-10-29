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
enum cell_type {EXPR, STRING, NUMBER};

using std::string;
using sexpr = std::vector<cell>;
using marker = sexpr::const_iterator;
using builtin = std::function<auto(marker, marker)->cell>;
using book = std::map<string, builtin>::iterator;

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int> content;

	cell(string s) : content{std::move(s)} { type = STRING; }
	cell(int n) : content{std::move(n)} { type = NUMBER; }
	cell(sexpr s) : content{std::move(s)} { type = EXPR; }
};

//Library access
std::map<string, builtin> library[3];
void build_library();

builtin check_shelf(string name, std::vector<cell_type> type) {
	int i = 0;
	do {
		book b = library[type[i]].find(name);
		if(b != library[type[i]].end())
			return b->second;
		i++;
	} while(type[i - 1] != EXPR);
	throw std::invalid_argument{name + " not in the library"};
}

builtin search_library(string name, cell_type type) {
	switch(type) {
		case STRING: case EXPR: 
			return check_shelf(name, {STRING, NUMBER, EXPR});
		case NUMBER:
			return check_shelf(name, {NUMBER, STRING, EXPR});
	}
	throw std::invalid_argument{name + " not in the library"};
}

//Base eval function
cell eval(sexpr const &c, cell_type type);
cell eval(cell const &c, cell_type type) {
	if(c.type == EXPR)
    	return std::visit([](auto const &c) { return eval(c, EXPR); }, c.content);
    return c;
}

//Convert to string
string str_eval(cell const &c) {
	if(c.type == STRING)
		return std::get<string>(c.content);
	if(c.type == NUMBER)
		return std::to_string(std::get<int>(c.content));
	return str_eval(eval(c, STRING));
}

//Convert to number
int num_eval(cell const &c) { 
	if(c.type == NUMBER)
		return std::get<int>(c.content);
	if(c.type == STRING) {
		string s = std::get<string>(c.content);
		if(isdigit(s[0]) || s[0] == '-')
			return std::stoi(s);
		if(s[0] == 't' || s[0] == 'T')
			return 1;
		return 0;
	}
	return num_eval(eval(c, NUMBER));
}

//Actual expression evaluation
cell eval(sexpr const &s, cell_type type) {
	if(s.begin() == s.end())
		throw std::domain_error("Empty sexpr");
	else {
		string fname = str_eval(*s.begin());
		builtin fp = search_library(fname, type);
		return fp(++s.begin(), s.end());
	}
}

//Test main function
int main() {
	try {
		using namespace std::string_literals;

		//Build test tree
		auto const prog = sexpr{
			"+"s,
			sexpr{"-"s, 30, 14},
			sexpr{"if"s,
				sexpr{"*"s, "true"s, "true"s},
				" True "s, " False "s
			},
			"Hello "s, "world!"s
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