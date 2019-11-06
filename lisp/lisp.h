#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <stdexcept>
#include <list>
#include <iostream>
#include <fstream>

/*
 * Created by Stuart Irwin on 28/10/2019.
 * Base list structures and systems
 * Based on https://gist.github.com/KayEss/45a2e88675832228f57e2d598afc02ae and https://gist.github.com/ofan/721464
 */

//Main data types
struct cell;
using std::string;
using sexpr = std::vector<cell>;
using marker = sexpr::const_iterator;
using builtin = std::function<auto(marker, marker)->cell>;

//Allow for simulated additional types
#ifndef type_count
#define type_count 4
#define addons false

//Base type lists
enum cell_type {EXPR, STRING, NUMBER, LIST};
cell_type type_conversions[type_count][type_count] = {
	{NUMBER, STRING, LIST, EXPR},
	{STRING, NUMBER, LIST, EXPR},
	{NUMBER, STRING, EXPR},
	{LIST, STRING, EXPR}
};

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, cell_type t = STRING) : content{std::move(s)} { type = t; }
	cell(int n, cell_type t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, cell_type t = EXPR) : content{std::move(s)} { type = t; }
};

#endif

//Allow additional type conversions
string str_eval_cont(cell const &c);
int num_eval_cont(cell const &c);
sexpr list_eval_cont(cell const &c);
void build_library_cont();

//Variable storage
std::map<string, cell> env;

//Library structure
std::map<string, builtin> library[type_count];
void build_library();

//Retrieving functions from library
builtin search_library(string name, cell_type type) {
	int i = 0;
	do {
		auto b = library[type_conversions[type][i]].find(name);
		if(b != library[type_conversions[type][i]].end())
			return b->second;
		i++;
	} while(type_conversions[type][i - 1] != EXPR);
	throw std::invalid_argument{name + " not in the library for type " + std::to_string(type)};
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
	if(c.type == LIST) {
		string output;
		sexpr vec = std::get<sexpr>(c.content);
		for(cell s : vec)
			output += str_eval(s) + " ";
		return output;
	}
	if(c.type == EXPR)
		return str_eval(eval(c, STRING));
	if(addons)
		return str_eval_cont(c);
	throw std::domain_error("Cannot convert to string from type " + std::to_string(c.type));
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
	if(addons)
		return num_eval_cont(c);
	throw std::domain_error("Cannot convert to number from type " + std::to_string(c.type));
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
	}
	if(c.type == EXPR)
		return list_eval(eval(c, LIST));
	if(addons)
		return list_eval_cont(c);

	//Convert to single object list
	sexpr *output = new sexpr();
	output->push_back(c);
	return *output;
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

//Convert given string to list of tokens
std::list<std::string> tokenize(const std::string & str) {
    std::list<std::string> tokens;
    const char *s = str.c_str();
    while(*s) {
        while (*s == ' ' || *s == '\t')
            ++s;
        if(*s == '(' || *s == ')')
            tokens.push_back(*s++ == '(' ? "(" : ")");
        else if(*s == '{' || *s == '}')
            tokens.push_back(*s++ == '{' ? "{" : "}");
        else if(*s == '"') {
        	const char *t = ++s;
            while(*t && *t != '"')
                ++t;
            tokens.push_back(std::string(s, t));
            s = ++t;
        } else {
            const char * t = s;
            while(*t && *t != ' ' && *t != '\t' && *t != '(' && *t != ')' && *t != '{' && *t != '}')
                ++t;
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    return tokens;
}

//Numbers become Numbers; every other token is a String
cell atom(const std::string & token) {
    if(isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        return cell(stoi(token));
    return cell(token);
}

//Return the Lisp expression in the given tokens
cell read_from(std::list<std::string> & tokens) {
    const std::string token(tokens.front());
    cell_type type = (token == "{") ? LIST : EXPR;
    tokens.pop_front();
    if(token == "(" || token == "{") {
        sexpr *output = new sexpr();
        while(tokens.front() != ")" && tokens.front() != "}")
            output->push_back(read_from(tokens));
        tokens.pop_front();
        return cell(*output, type);
    }
    else
        return atom(token);
}

//Return the Lisp expression represented by the given string
cell read(const std::string & s) {
    std::list<std::string> tokens(tokenize(s));
    return read_from(tokens);
}

//The default read-eval-print-loop
void repl(const std::string &prompt, std::istream &in) {
	while(!in.eof()) {
		std::cout << prompt;
		std::string object;
		int levels = -1;
		bool literal = false;

		//Count parenthesis
		while(levels != 0 && !in.eof()) {
			bool comment = false;
			std::string line;
			std::getline(in, line);
			if(line.length() > 0) {
				if(levels == -1)
					levels = 0;

				//Each letter
				for(char c : line) {
					if(!literal && !comment) {
						if(c == '(' || c == '{') {
							levels++;
						} else if(c == ')' || c == '}') {
							levels--;
						} else if(c == '#') {
							if(object.length() > 0)
								object += ' ';
							comment = true;
						} 
					}
					if(c == '"')
						literal = !literal;
					if(!comment)
						object += c;
				}
				if(!comment)
					object += ' ';
			}
		}

		try {
			if(object.length() > 0) {
				if(in.eof() && levels != 0)
					throw std::domain_error(std::to_string(levels) + " non matched parenthesis");
				std::cout << str_eval(read(object)) << '\n';
			}
		} catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}