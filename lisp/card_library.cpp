#include "card_env.h"

/*
 * Created by Stuart Irwin on 4/11/2019.
 * library functions for Solitaire lisp system.
 */

//Build a function to set the suit of a list of cards
cell CardEnviroment::setSuits(string suit) {
	return cell([suit](Enviroment *env, marker pos, marker end) {
		LISTREMAINS;
		sexpr output;

		//Add all cards to new deck
		while(pos != end)
			output.push_back(cell(suit + std::to_string(env->num_eval(*pos++)), CARD));

		return cell(output, DECK);
	});
}

cell CardEnviroment::buildLayout(layout_type index) {
	return cell([index](Enviroment *env, marker pos, marker end) {
		LISTREMAINS;
		sexpr output;

		output.push_back(index);
		while(pos != end)
			output.push_back(*pos++);

		//output.push_back(cell(array, LIST));

		return cell(output, LAYOUT);
	});
}

void CardEnviroment::build_library_cont() {
	//Set global lisp variables
	sexpr standard;
	standard.reserve(13);
	for(int i = 1; i <= 13; i++)
		standard.push_back(cell("N" + std::to_string(i), CARD));
	set("Standard", cell(standard, DECK));

	//Build force evaluators
	force_eval[CARD] = [](Enviroment *env, cell const &c) {
		return cell(cenv->to_string(cenv->card_eval(c)), CARD);
	};
	force_eval[DECK] = [](Enviroment *env, cell const &c) {
		return cell(cenv->deck_eval(c), DECK);
	};
	force_eval[FILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->filter_eval(c), FILTER);
	};
	force_eval[TAGFILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->tagfilter_eval(c, CLOSED), TAGFILTER);
	};
	force_eval[LAYOUT] = [](Enviroment *env, cell const &c) {
		return cell(cenv->layout_eval(c), LAYOUT);
	};

	//Base filter types
	set("Filter", cell([](Enviroment *env, marker pos, marker end) {
		cell output = cell(cenv->tagfilter_eval(*pos++, CLOSED), TAGFILTER);
		DONE;
		return output;
	}));
	set("Filter-Open", cell([](Enviroment *env, marker pos, marker end) {
		cell output = cell(cenv->tagfilter_eval(*pos++, OPEN), TAGFILTER);
		DONE;
		return output;
	}));
	set("Filter-All", cell([](Enviroment *env, marker pos, marker end) {
		cell output = cell(cenv->tagfilter_eval(*pos++, ALL), TAGFILTER);
		DONE;
		return output;
	}));

	//Set up special filters
	set("Four-Suit", cell([](Enviroment *env, marker pos, marker end) {
		string suits[4] = {"Hearts", "Spades", "Diamonds", "Clubs"};
		sexpr output;

		//Copy all cards into functions for each suit
		for(string s : suits) {
			sexpr deck;
			deck.push_back(cell(s));
			deck.push_back(*pos);
			output.push_back(cell(deck, EXPR));
		}

		pos++;
		DONE;
		return cell(output, FILTER);
	}));
	set("Alternating", cell([](Enviroment *env, marker pos, marker end) {
		sexpr array = cenv->deck_eval(*pos++);
		sexpr output;

		//Copy all cards into alternating colors
		bool red = false;
		for(int i = 0; i < 2; i++) {
			sexpr deck;
			for(cell c : array) {
				//Set each card color
				//cardData d = cenv->card_eval(c);
				string suit = red ? "R" : "B";
				red = !red;
				deck.push_back(cell(suit + std::to_string(env->num_eval(c)), CARD));
			}
			output.push_back(cell(deck, DECK));
			red = true;
		}

		return cell(output, FILTER);
	}));

	//Change suit of full deck
	set("Hearts", setSuits("H"));
	set("Spades", setSuits("S"));
	set("Diamonds", setSuits("D"));
	set("Clubs", setSuits("C"));
	set("Red", setSuits("R"));
	set("Black", setSuits("B"));

	//Create layout to hold slots
	set("VLayout", buildLayout(VLayout));
	set("HLayout", buildLayout(HLayout));
	set("VStack", buildLayout(VStack));
	set("HStack", buildLayout(HStack));
	set("PStack", buildLayout(PStack));
	set("Slot", buildLayout(Slot));

	//Apply tags to all contained slots
	set("Apply", cell([](Enviroment *env, marker pos, marker end) {
		sexpr output;
		output.push_back(Apply);
		output.push_back(*pos++);
		output.push_back(*pos++);

		DONE;
		return cell(output, LAYOUT);
	}));

	//Grid layout of specific width
	set("GLayout", cell([](Enviroment *env, marker pos, marker end) {
		int row_length = env->num_eval(*pos++);
		sexpr output;
		output.push_back(VLayout);

		//Loop through all rows
		while(pos != end) {
			sexpr array;
			array.push_back(cell("HLayout"));
			for(int i = 0; i < row_length && pos != end; i++)
				array.push_back(*pos++);
			output.push_back(cell(array, EXPR));
		}

		return cell(output, LAYOUT);
	}));
}