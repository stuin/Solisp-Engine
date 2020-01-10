#include "card_lisp.h"

/*
 * Created by Stuart Irwin on 4/11/2019.
 * Library functions for Solitaire lisp system.
 */

void CardEnviroment::build_library_cont() {
	//Set global lisp variables
	sexpr *standard = new sexpr();
	standard->reserve(13);
	for(int i = 1; i <= 13; i++)
		standard->push_back(to_card("N" + std::to_string(i)));
	vars["Standard"] = cell(*standard, DECK);

	//Build force evaluators
	force_eval[CARD] = [](Enviroment *env, cell const &c) {
		return cell(cenv->card_eval(c), CARD);
	};
	force_eval[DECK] = [](Enviroment *env, cell const &c) {
		return cell(cenv->deck_eval(c), DECK);
	};
	force_eval[FILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->filter_eval(c), FILTER);
	};
	force_eval[TAGFILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->tagfilter_eval(c, false), TAGFILTER);
	};
	force_eval[LAYOUT] = [](Enviroment *env, cell const &c) {
		return cell(cenv->layout_eval(c), LAYOUT);
	};

	//Link force evaluators
	library["Card"] = forcer(CARD);
	library["Deck"] = forcer(DECK);
	library["Filter"] = forcer(TAGFILTER);
	library["Filter-All"] = library["Filter"];

	library["Filter-Open"] = [](Enviroment* env, marker pos, marker end) {
		cell output = cell(cenv->tagfilter_eval(*pos++, true), TAGFILTER);
		DONE;
		return output;
	};

	//Set up special filters
	library["Four-Suit"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = cenv->deck_eval(*pos++);
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
	library["Alternating"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = cenv->deck_eval(*pos++);
		sexpr *output = new sexpr();

		//Copy all cards into alternating colors
		bool red = false;
		for(int i = 0; i < 2; i++) {
			sexpr *deck = new sexpr();
			for(cell c : array) {
				//Set each card color
				cardData d = cenv->card_eval(c);
				d.suit = red ? 'R' : 'B';
				red = !red;
				deck->push_back(d);
			}
			output->push_back(cell(*deck, DECK));
			red = true;
		}

		return cell(*output, FILTER);
	};

	//Change suit of full deck
	library["Hearts"] = setSuits('H');
	library["Spades"] = setSuits('S');
	library["Diamonds"] = setSuits('D');
	library["Clubs"] = setSuits('C');
	library["Red"] = setSuits('R');
	library["Black"] = setSuits('B');

	//Create layout to hold slots
	library["VLayout"] = buildLayout(VLayout);
	library["HLayout"] = buildLayout(HLayout);
	library["VStack"] = buildLayout(VStack);
	library["HStack"] = buildLayout(HStack);
	library["PStack"] = buildLayout(PStack);
	library["Slot"] = buildLayout(Slot);

	//Apply tags to all contained slots
	library["Apply"] = [](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(Apply);
		output->push_back(*pos++);
		output->push_back(*pos++);

		DONE;
		return cell(*output, LAYOUT);
	};
	//Duplicate layout multiple times
	library["*"] = [](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(Multiply);
		output->push_back(*pos++);
		output->push_back(*pos++);

		DONE;
		return cell(*output, LAYOUT);
	};

	//Grid layout of specific width
	library["GLayout"] = [](Enviroment *env, marker pos, marker end) {
		int row_length = env->num_eval(*pos++);
		sexpr *output = new sexpr();
		output->push_back(VLayout);

		//Loop through all rows
		while(pos != end) {
			sexpr *array = new sexpr();
			array->push_back(cell("HLayout"));
			for(int i = 0; i < row_length && pos != end; i++)
				array->push_back(*pos++);
			output->push_back(cell(*array, EXPR));
		}

		return cell(*output, LAYOUT);
	};
}