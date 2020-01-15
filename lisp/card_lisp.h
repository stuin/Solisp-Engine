#include "card.h"
#include "lisp.h"

#include <vector>
#include <string>
#include <variant>

#include <iostream>

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and convertions
 */

//Additional type definitions
using Solisp::cardData;
enum layout_type { VLayout, HLayout, GLayout, VStack, HStack, PStack, Slot, Apply, Multiply};

#define cenv ((CardEnviroment*)env)

//Added cell types
#define CARD LIST+1
#define DECK LIST+2
#define FILTER LIST+3
#define TAGFILTER LIST+4
#define LAYOUT LIST+5

class CardEnviroment : public Enviroment {
private:
	//Card builtin generators
	builtin setSuits(string suit);
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