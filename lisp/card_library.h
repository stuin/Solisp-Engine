#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 4/11/2019.
 * Library functions for Solitaire lisp system.
 */

//Build a function to set the suit of a list of cards
builtin setSuit(char suit) {
	return [suit](marker pos, marker end) {
		sexpr *output = new sexpr();
		sexpr array = deck_eval(*pos++);

		//Add all cards to new deck
		for(cell c : array) {
			cardData d = card_eval(c);
			d.suit = suit;
			output->push_back(d);
		}

		DONE;
		return cell(*output, DECK);
	};
}

builtin buildLayout(int index) {
	return [index](marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(index);
		output->push_back(list_eval(*pos++));
		DONE;
		return cell(*output, LAYOUT);
	};
}

//Set global lisp variables
void build_variables() {
	sexpr *standard = new sexpr();
	standard->reserve(13);
	for(int i = 1; i <= 13; i++)
		standard->push_back(to_card("N" + std::to_string(i)));
	env["Standard"] = cell(*standard, DECK);
}

void build_library_cont() {
	build_variables();

	//Build force evaluators
	set_force_eval(&card_eval, CARD);
	set_force_eval(&deck_eval, DECK);
	set_force_eval(&filter_eval, FILTER);
	set_force_eval(&layout_eval, LAYOUT);

	//Link force evaluators
	library[CARD]["Card"] = forcer(CARD);
	library[DECK]["Deck"] = forcer(DECK);
	library[FILTER]["Filter"] = forcer(FILTER);

	//Set up special filters
	library[FILTER]["Four-Suit"] = [](marker pos, marker end) {
		sexpr array = deck_eval(*pos++);
		sexpr *output = new sexpr();
		string suits[4] = {"Hearts", "Spades", "Diamonds", "Clubs"};

		//Copy all cards into functions for each suit
		for(string s : suits) {
			sexpr *deck = new sexpr();
			deck->push_back(cell(s));
			deck->push_back(cell(array, DECK));
			output->push_back(cell(*deck, EXPR));
		}

		return cell(*output, FILTER);
	};
	library[FILTER]["Alternating"] = [](marker pos, marker end) {
		sexpr array = deck_eval(*pos++);
		sexpr *output = new sexpr();

		//Copy all cards into alternating colors
		bool red = false;
		for(int i = 0; i < 2; i++) {
			sexpr *deck = new sexpr();
			for(cell c : array) {
				//Set each card color
				cardData d = card_eval(c);
				d.suit = red ? 'R' : 'B';
				red = !red;
				deck->push_back(d);
			}
			output->push_back(cell(*deck, DECK));
			red = true;
		}

		return cell(*output, FILTER);
	};

	//Change suit of single deck
	library[DECK]["Hearts"] = setSuit('H');
	library[DECK]["Spades"] = setSuit('S');
	library[DECK]["Diamonds"] = setSuit('D');
	library[DECK]["Clubs"] = setSuit('C');
	library[DECK]["Red"] = setSuit('R');
	library[DECK]["Black"] = setSuit('B');

	//Create layout to hold slots
	library[LAYOUT]["VLayout"] = buildLayout(-1);
	library[LAYOUT]["HLayout"] = buildLayout(-2);
	library[LAYOUT]["VStack"] = buildLayout(1);
	library[LAYOUT]["HStack"] = buildLayout(2);
	library[LAYOUT]["PStack"] = buildLayout(3);
	library[LAYOUT]["Slot"] = buildLayout(4);

	//Grid layout of specific width
	library[LAYOUT]["GLayout"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(-3);
		output->push_back(num_eval(*pos++));
		output->push_back(list_eval(*pos++));
		DONE;
		return cell(*output, LAYOUT);
	};

	//Apply tags to all contained slots
	library[LAYOUT]["Apply"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(-4);
		output->push_back(list_eval(*pos++));
		output->push_back(list_eval(*pos++));

		DONE;
		return cell(*output, LAYOUT);
	};
}