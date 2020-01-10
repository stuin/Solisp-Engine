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

#define cenv ((CardEnviroment*)env)

enum layout_type { VLayout, HLayout, GLayout, VStack, HStack, PStack, Slot, Apply, Multiply};

//Expanded type lists
#define type_count 10
enum cell_type {EXPR, STRING, NUMBER, CHAR, LIST, CARD, DECK, FILTER, TAGFILTER, LAYOUT};

//Main data sructure
struct cell {
	cell_type type;
	std::variant<sexpr, string, int, char, cardData> content;

	//Constructors
	cell() { cell(""); }
	cell(string s, cell_type t = STRING) : content{std::move(s)} { type = t; }
	cell(int n, cell_type t = NUMBER) : content{std::move(n)} { type = t; }
	cell(char c, cell_type t = CHAR) : content{std::move(c)} { type = t; }
	cell(sexpr s, cell_type t = EXPR) : content{std::move(s)} { type = t; }
	cell(cardData c, cell_type t = CARD) : content{std::move(c)} { type = t; }

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

	//Convert cell types
	cardData card_eval(cell const &c);
	sexpr deck_eval(cell const &c);
	sexpr filter_eval(cell const &c);
	sexpr tagfilter_eval(cell const &c, bool open=false);
	sexpr layout_eval(cell const &c);

	CardEnviroment() {
		addons = true;
		build_library_cont();
	}
};