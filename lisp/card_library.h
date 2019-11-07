#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 4/11/2019.
 * Library functions for Solitaire lisp system.
 */

builtin setSuit(char suit) {
	return [suit](marker pos, marker end) {
		sexpr *output = new sexpr();
		sexpr array = deck_eval(*pos++);

		//Add all cards to new deck
		for(cell c : array) {
			card d = card_eval(c);
			d.suit = suit;
			output->push_back(d);
		}

		DONE;
		return cell(*output, DECK);
	};
}

void build_variables() {
	sexpr *standard = new sexpr();
	standard->reserve(13);
	for(int i = 1; i <= 13; i++)
		standard->push_back(to_card("N" + std::to_string(i)));
	env["Standard"] = cell(*standard, DECK);
}

void build_library_cont() {
	build_variables();

	//Set force evaluators
	set_force_eval(&card_eval, CARD);
	set_force_eval(&deck_eval, DECK);
	set_force_eval(&filter_eval, FILTER);

	//Link force evaluators
	library[CARD]["Card"] = forcer(CARD);
	library[DECK]["Deck"] = forcer(DECK);
	library[FILTER]["Filter"] = forcer(FILTER);
	library[FILTER]["Filter-Open"] = library[FILTER]["Filter"];

	//Set up special filters
	library[FILTER]["Four-Suit"] = [](marker pos, marker end) {
		sexpr array = deck_eval(*pos++);
		sexpr *output = new sexpr();

		sexpr *deck = new sexpr();
		deck->push_back(cell("Hearts"));
		deck->push_back(cell(array, DECK));
		output->push_back(cell(*deck, EXPR));

		deck = new sexpr();
		deck->push_back(cell("Spades"));
		deck->push_back(cell(array, DECK));
		output->push_back(cell(*deck, EXPR));

		deck = new sexpr();
		deck->push_back(cell("Diamonds"));
		deck->push_back(cell(array, DECK));
		output->push_back(cell(*deck, EXPR));

		deck = new sexpr();
		deck->push_back(cell("Clubs"));
		deck->push_back(cell(array, DECK));
		output->push_back(cell(*deck, EXPR));

		return cell(*output, FILTER);
	};
	library[FILTER]["Alternating"] = [](marker pos, marker end) {
		sexpr array = deck_eval(*pos++);
		sexpr *output = new sexpr();

		bool red = false;
		sexpr *deck = new sexpr();
		for(cell c : array) {
			card d = card_eval(c);
			d.suit = red ? 'R' : 'B';
			red = !red;
			deck->push_back(d);
		}
		output->push_back(cell(*deck, DECK));

		red = true;
		deck = new sexpr();
		for(cell c : array) {
			card d = card_eval(c);
			d.suit = red ? 'R' : 'B';
			red = !red;
			deck->push_back(d);
		}
		output->push_back(cell(*deck, DECK));

		return cell(*output, FILTER);
	};

	//Change suit of deck
	library[DECK]["Hearts"] = setSuit('H');
	library[DECK]["Spades"] = setSuit('S');
	library[DECK]["Diamonds"] = setSuit('D');
	library[DECK]["Clubs"] = setSuit('C');
	library[DECK]["Red"] = setSuit('R');
	library[DECK]["Black"] = setSuit('B');
}