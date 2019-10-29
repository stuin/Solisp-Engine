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
enum cell_type {Expr, String, Number};

using sexpr = std::vector<cell>;
using marker = sexpr::const_iterator;
using builtin = std::function<auto(marker, marker)->cell>;
using std::string;

//Main data sructure
struct cell {
	cell_type type;
	std::variant<string, sexpr, int> content;

	cell(string s) : content{std::move(s)} { type = String; }
	cell(int n) : content{std::move(n)} { type = Number; }
	cell(sexpr s) : content{std::move(s)} { type = Expr; }
};
std::map<string, builtin> library;

//Base eval function
cell eval(sexpr const &);
cell eval(cell const &c) {
	if(c.type == Expr)
    	return std::visit([](auto const &c) { return eval(c); }, c.content);
    return c;
}

//Convert to string
string str_eval(cell const &c) {
	if(c.type == String)
		return std::get<string>(c.content);
	if(c.type == Number)
		return std::to_string(std::get<int>(c.content));
	return str_eval(eval(c));
}

//Convert to number
int num_eval(cell const &c) { 
	if(c.type == Number)
		return std::get<int>(c.content);
	if(c.type == String) {
		string s = std::get<string>(c.content);
		if(isdigit(s[0]) || s[0] == '-')
			return std::stoi(s);
		if(s[0] == 't' || s[0] == 'T')
			return 1;
		return 0;
	}
	return num_eval(eval(c));
}

//Actual expression evaluation
cell eval(sexpr const &s) {
	if(s.begin() == s.end())
		throw std::domain_error("Empty sexpr");
	else {
		auto const fname = str_eval(*s.begin());
		if(auto const fp = library.find(fname); fp != library.end())
			return fp->second(++s.begin(), s.end());
		else
			throw std::invalid_argument{fname + " not in the library"};
	}
}

//Test main function
int main() {
	try {
		using namespace std::string_literals;

		//Build test tree
		auto const prog = sexpr{
			"cat"s,
			sexpr{"-"s, 30, 14},
			sexpr{"if"s,
				sexpr{"*"s, "true"s, "true"s},
				" True "s, " False "s
			},
			"Hello "s, "world!"s
		};

		//Add base test functions
		library["cat"] = [](marker pos, marker end) {
			string ret;
			while (pos != end) ret += str_eval(*pos++);
			return cell(ret);
		};
		library["+"] = [](marker pos, marker end) {
			int ret = 0;
			while(pos != end) ret += num_eval(*pos++);
			return cell(ret);
		};
		library["-"] = [](marker pos, marker end) {
			int ret = num_eval(*pos++);
			while(pos != end) ret -= num_eval(*pos++);
			return cell(ret);
		};
		library["*"] = [](marker pos, marker end) {
			int ret = 1;
			while(pos != end) ret *= num_eval(*pos++);
			return cell(ret);
		};
		library["if"] = [](marker pos, marker end) {
			if(num_eval(*pos++))
				return cell(eval(*pos));
			else {
				pos++;
				return cell(eval(*pos));
			}
		};

		//Run test code
		std::cout << str_eval(prog) << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}