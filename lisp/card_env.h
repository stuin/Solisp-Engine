#include "card.h"
#include "Solisp/src/lisp.h"

#include <sstream>

/*
 * Created by Stuart Irwin on 1/11/2019.
 * Solitaire lisp types and convertions
 */

//Additional type definitions
using Solisp::cardData;
enum layout_type { VLayout, HLayout, GLayout, VStack, HStack, PStack, Slot, Apply};

#define cenv ((CardEnviroment*)env)

//Added cell types
#define CARD SOL_LIST+1
#define DECK SOL_LIST+2
#define FILTER SOL_LIST+3
#define TAGFILTER SOL_LIST+4
#define LAYOUT SOL_LIST+5

class CardEnviroment : public Enviroment {
private:
	//Card builtin generators
	cell setSuits(string suit);
	cell buildLayout(layout_type index);

	void build_library_cards();
public:
	//Convert stored string to card
	cardData to_card(string s);
	string to_string(cardData card);

	//Extend existing cell types
	string str_eval_cont(cell const &c, bool literal=false) override;
	int num_eval_cont(cell const &c) override;
	sexpr list_eval_cont(cell const &c) override;

	//Convert cell types
	cardData card_eval(cell const &c);
	sexpr deck_eval(cell const &c);
	sexpr filter_eval(cell const &c);
	sexpr tagfilter_eval(cell const &c, filter_type open=CLOSED);
	sexpr layout_eval(cell const &c);

	CardEnviroment() {
		build_library_cards();
		shift_env(true);
	}
};