#pragma once

#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <stdexcept>
#include <list>
#include <iostream>
#include <fstream>
#include <initializer_list>

/*
 * Created by Stuart Irwin on 28/10/2019.
 * Base list structures and systems
 * Loosely based on https://gist.github.com/KayEss/45a2e88675832228f57e2d598afc02ae and https://gist.github.com/ofan/721464
 */

//Main simple data types
struct cell;
class Enviroment;
using std::string;
using sexpr = std::vector<cell>;
using marker = sexpr::const_iterator;
using builtin = std::function<auto(Enviroment*, marker, marker)->cell>;
using force_builtin = std::function<cell(Enviroment*, const cell&)>;

//End of builtin function macro
#define DONE if(pos != end) throw std::domain_error("Too many arguments: " + env->str_eval(*pos, true));

//Allow for adding specialized types
#ifndef type_count
#define type_count 5
#define addons false

//Base type lists
enum cell_type {EXPR, STRING, NUMBER, CHAR, LIST};

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, char, int> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, cell_type t = STRING) : content{std::move(s)} { type = t; }
	cell(char c, cell_type t = CHAR) : content{std::move(c)} { type = t; }
	cell(int n, cell_type t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, cell_type t = EXPR) : content{std::move(s)} { type = t; }

	//Equality
	friend bool operator==(const cell &first, const cell &second) {
		return first.content == second.content;
	}
};

#endif

//Enviroment of system variables
class Enviroment {
private:
	int start_line = 0;

	//Library structure
	void build_library();

	//Reader functions
	std::list<std::string> tokenize(const std::string & str);
	cell atom(const std::string & token);
	cell read_from(std::list<std::string> & tokens);

	//Lambda builder functions
	template <class T> builtin comparitor(T func);
	template <class T> builtin arithmetic(T func);

public:
	bool addons = false;
	force_builtin force_eval[type_count];
	std::map<string, builtin> library;
	std::map<string, cell> vars;

	//Type forcing conversions
	builtin forcer(cell_type type);

	//Allow additional type conversions
	string str_eval_cont(cell const &c, bool literal=false);
	int num_eval_cont(cell const &c);
	char char_eval_cont(cell const &c);
	sexpr list_eval_cont(cell const &c);

	//Base eval function
	cell eval(sexpr const &c, cell_type type);
	cell eval(cell const &c, cell_type type);

	//Convert types
	string str_eval(cell const &c, bool literal=false);
	int num_eval(cell const &c);
	char char_eval(cell const &c);
	sexpr list_eval(cell const &c);

	//Public reader functions
	cell read(const std::string & s);
	cell read_stream(std::istream &in, cell_type type, int new_line = -1);

	Enviroment() {
		build_library();
	}
};