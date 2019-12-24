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
		return cell(((CardEnviroment*)env)->card_eval(c), CARD);
	};
	force_eval[DECK] = [](Enviroment *env, cell const &c) {
		return cell(((CardEnviroment*)env)->deck_eval(c), DECK);
	};
	force_eval[FILTER] = [](Enviroment *env, cell const &c) {
		return cell(((CardEnviroment*)env)->filter_eval(c), FILTER);
	};
	force_eval[LAYOUT] = [](Enviroment *env, cell const &c) {
		return cell(((CardEnviroment*)env)->layout_eval(c), LAYOUT);
	};

	//Link force evaluators
	library[CARD]["Card"] = forcer(CARD);
	library[DECK]["Deck"] = forcer(DECK);
	library[FILTER]["Filter"] = forcer(FILTER);
	library[FILTER]["Filter-Open"] = library[FILTER]["Filter"];
	library[FILTER]["Filter-All"] = library[FILTER]["Filter"];

	//Set up special filters
	library[FILTER]["Four-Suit"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = ((CardEnviroment*)env)->deck_eval(*pos++);
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
	library[FILTER]["Alternating"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = ((CardEnviroment*)env)->deck_eval(*pos++);
		sexpr *output = new sexpr();

		//Copy all cards into alternating colors
		bool red = false;
		for(int i = 0; i < 2; i++) {
			sexpr *deck = new sexpr();
			for(cell c : array) {
				//Set each card color
				cardData d = ((CardEnviroment*)env)->card_eval(c);
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
	library[DECK]["Hearts"] = setSuits('H');
	library[DECK]["Spades"] = setSuits('S');
	library[DECK]["Diamonds"] = setSuits('D');
	library[DECK]["Clubs"] = setSuits('C');
	library[DECK]["Red"] = setSuits('R');
	library[DECK]["Black"] = setSuits('B');

	//Change suit of single card
	/*library[CARD]["Hearts"] = setSuit('H');
	library[CARD]["Spades"] = setSuit('S');
	library[CARD]["Diamonds"] = setSuit('D');
	library[CARD]["Clubs"] = setSuit('C');
	library[CARD]["Red"] = setSuit('R');
	library[CARD]["Black"] = setSuit('B');*/

	//Create layout to hold slots
	library[LAYOUT]["VLayout"] = buildLayout(VLayout);
	library[LAYOUT]["HLayout"] = buildLayout(HLayout);
	library[LAYOUT]["VStack"] = buildLayout(VStack);
	library[LAYOUT]["HStack"] = buildLayout(HStack);
	library[LAYOUT]["PStack"] = buildLayout(PStack);
	library[LAYOUT]["Slot"] = buildLayout(Slot);

	//Grid layout of specific width
	library[LAYOUT]["GLayout"] = [](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(GLayout);
		output->push_back(env->num_eval(*pos++));
		output->push_back(*pos++);
		DONE;
		return cell(*output, LAYOUT);
	};

	//Apply tags to all contained slots
	library[LAYOUT]["Apply"] = [](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(Apply);
		output->push_back(*pos++);
		output->push_back(*pos++);

		DONE;
		return cell(*output, LAYOUT);
	};
	//Duplicate layout multiple times
	library[LAYOUT]["*"] = [](Enviroment *env, marker pos, marker end) {
		sexpr *output = new sexpr();
		output->push_back(Multiply);
		output->push_back(*pos++);
		output->push_back(*pos++);
		
		DONE;
		return cell(*output, LAYOUT);
	};
}