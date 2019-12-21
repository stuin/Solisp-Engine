#include "card.h"

#include <vector>
#include <string>
#include <variant>

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and convertions
 */

//Required early type definitions
struct cell;
using std::string;
using sexpr = std::vector<cell>;
using Solisp::cardData;

//Expanded type lists
#define type_count 8
enum cell_type {EXPR, STRING, NUMBER, LIST, CARD, DECK, FILTER, LAYOUT};
cell_type type_conversions[type_count][type_count] = {
	{NUMBER, STRING, LIST, CARD, DECK, FILTER, LAYOUT, EXPR},
	{STRING, NUMBER, LIST, CARD, DECK, FILTER, LAYOUT, EXPR},
	{NUMBER, STRING, CARD, EXPR},
	{LIST, DECK, FILTER, NUMBER, STRING, CARD, LAYOUT, EXPR},
	{CARD, NUMBER, STRING, EXPR},
	{DECK, FILTER, LIST, CARD, NUMBER, STRING, EXPR},
	{FILTER, DECK, LIST, CARD, NUMBER, STRING, EXPR},
	{LAYOUT, LIST, STRING, EXPR}
};

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int, cardData> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, cell_type t = STRING) : content{std::move(s)} { type = t; }
	cell(int n, cell_type t = NUMBER) : content{std::move(n)} { type = t; }
	cell(sexpr s, cell_type t = EXPR) : content{std::move(s)} { type = t; }
	cell(cardData c, cell_type t = CARD) : content{std::move(c)} { type = t; }

	//Equality
	friend bool operator==(const cell &first, const cell &second) {
		return first.content == second.content;
	}
};

//Import main lisp system
#define addons true
#include "lisp.h"

class CardEnviroment : public Enviroment {
private:
	//Convert stored string to card
	cardData to_card(string s);
	string to_string(cardData card);

public:
	//Convert cell types
	cardData card_eval(cell const &c);
	sexpr deck_eval(cell const &c);
	sexpr filter_eval(cell const &c);
	sexpr layout_eval(cell const &c);
}