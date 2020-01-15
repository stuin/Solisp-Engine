#include "card.h"

#include <vector>
#include <string>
#include <variant>

#include <iostream>

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and convertions
 */

//Required early type definitions
struct cell;
using std::string;
using sexpr = std::vector<cell>;
using Solisp::cardData;

#define cenv ((CardEnviroment*)env)

enum layout_type { VLayout, HLayout, GLayout, VStack, HStack, PStack, Slot, Apply, Multiply};

#define EXPR 0
#define STRING 1
#define NUMBER 2
#define CHAR 3
#define LIST 4

//Expanded type lists
#define CARD LIST+1
#define DECK LIST+2
#define FILTER LIST+3
#define TAGFILTER LIST+4
#define LAYOUT LIST+5

#define type_count 10

//Main data sructure
struct cell {
	int type;
	std::variant<sexpr, string, int, cardData> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, int t = STRING) : content{std::move(s)} { type = t; }
	//cell(char c, int t = CHAR) : content{std::move(c)} { type = t; }
	cell(int n, int t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, int t = EXPR) : content{std::move(s)} { type = t; }
	cell(cardData c, int t = CARD) : content{std::move(c)} { type = t; }

	//Equality
	friend bool operator==(const cell &first, const cell &second) {
		return first.content == second.content;
	}
};

//Import main lisp system
#include "lisp.h"

class CardEnviroment : public Enviroment {
private:
	//Card builtin generators
	builtin setSuits(char suit);
	builtin setSuit(char suit);
	builtin buildLayout(layout_type index);

	void build_library_cont();

public:
	//Convert stored string to card
	cardData to_card(string s);
	string to_string(cardData card);

	//Extend existing cell types
	string str_eval_cont(cell const &c, bool literal=false);
	int num_eval_cont(cell const &c);
	sexpr list_eval_cont(cell const &c);

	//Convert cell types
	cardData card_eval(cell const &c);
	sexpr deck_eval(cell const &c);
	sexpr filter_eval(cell const &c);
	sexpr tagfilter_eval(cell const &c, bool open=false);
	sexpr layout_eval(cell const &c);

	CardEnviroment() {
		build_library_cont();
	}
};