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

		return cell(output, LAYOUT);
	});
}

void CardEnviroment::build_library_cards() {
	//Set global lisp variables
	sexpr standard;
	standard.reserve(13);
	for(int i = 1; i <= 13; i++)
		standard.push_back(cell("N" + std::to_string(i), CARD));
	set("Standard", cell(standard, DECK));

	sexpr jokers = {cell("R14"), cell("B14") };
	set("Jokers", cell(jokers, DECK));

	set("Get-Max", *get("Max"));
	set("Max", cell("Max", STRING));

	//Read dealer structure in as string
	string dealer = "(HLayout (Slot Start-Extra BUTTON INPUT (On-Grab(Flip 3 (+ this 1) (+ this 2)) ";
	dealer += "(If (== (Count this) 0) (Move-All (+ this 2) this)) (Flip 3 (+ this 1) (+ this 2))))";
	dealer += "(HStack {(Max 3)}) (Slot INPUT OUTPUT))";
	//set("Dealer", read(dealer));

	//Build force evaluators
	type_name[CARD] = "Card";
	force_eval[CARD] = [](Enviroment *env, cell const &c) {
		return cell(cenv->to_string(cenv->card_eval(c)), CARD);
	};
	type_name[DECK] = "Deck";
	force_eval[DECK] = [](Enviroment *env, cell const &c) {
		return cell(cenv->deck_eval(c), DECK);
	};
	type_name[FILTER] = "Filter";
	force_eval[FILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->filter_eval(c), FILTER);
	};
	type_name[TAGFILTER] = "Tag-Filter";
	force_eval[TAGFILTER] = [](Enviroment *env, cell const &c) {
		return cell(cenv->tagfilter_eval(c, CLOSED), TAGFILTER);
	};
	type_name[LAYOUT] = "Layout";
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
			deck.push_back(cell(s, NAME));
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
			array.push_back(cell("HLayout", NAME));
			for(int i = 0; i < row_length && pos != end; i++)
				array.push_back(*pos++);
			output.push_back(cell(array, EXPR));
		}

		return cell(output, LAYOUT);
	}));
}