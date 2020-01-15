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

//Base data types
#define EXPR 0
#define STRING 1
#define NUMBER 2
#define CHAR 3
#define LIST 4

//Allow for adding specialized types
#ifndef type_count
#define type_count 5

//Main data sructure
struct cell {
	int type;
	std::variant<sexpr, string, int> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, int t = STRING) : content{std::move(s)} { type = t; }
	//cell(char c, int t = CHAR) : content{std::move(c)} { type = t; }
	cell(int n, int t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, int t = EXPR) : content{std::move(s)} { type = t; }

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
	force_builtin force_eval[type_count];
	std::map<string, builtin> library;
	std::map<string, cell> vars;

	//Type forcing conversions
	builtin forcer(int type);

	//Allow additional type conversions
	virtual string str_eval_cont(cell const &c, bool literal=false);
	virtual int num_eval_cont(cell const &c);
	virtual char char_eval_cont(cell const &c);
	virtual sexpr list_eval_cont(cell const &c);

	//Base eval function
	cell eval(sexpr const &c, int type);
	cell eval(cell const &c, int type);

	//Convert types
	string str_eval(cell const &c, bool literal=false);
	int num_eval(cell const &c);
	char char_eval(cell const &c);
	sexpr list_eval(cell const &c);

	//Public reader functions
	cell read(const std::string & s);
	cell read_stream(std::istream &in, int type, int new_line = -1);

	Enviroment() {
		build_library();
	}
};