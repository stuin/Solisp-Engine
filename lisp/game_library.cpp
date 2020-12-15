#include "../Gameplay/stack.h"
#include "../Gameplay/move.h"
#include "game_env.h"

/*
 * Created by Stuart Irwin on 28/1/2020.
 * Lisp structures for moving cards during game
 */

cell GameEnviroment::general_move(int num, bool flip, unc player) {
	return cell([num, player, flip](Enviroment *env, marker pos, marker end) {
		int from = env->num_eval(*pos++);
		int to = env->num_eval(*pos++);
		int count = num;

		//If number included
		if(pos != end) {
			count = from;
			from = to;
			to = env->num_eval(*pos++);
		}

		DONE;
		if(genv->both_valid(from, to)) {
			genv->add_move(count, from, to, player, flip);
			return cell(count);
		}
		return cell(0);
	});
}

void GameEnviroment::build_library_game() {
	//Base moves
	set("Move", general_move(1, false, 0));
	set("Move-All", general_move(1000, false, 0));

	//Flip moves
	set("Flip", general_move(1, true, 0));
	set("Flip-All", general_move(1000, true, 0));

	//Soft moves
	set("Soft-Move", general_move(1, false, 1));
	set("Soft-Flip", general_move(1, true, 1));

	//Count cards in stack
	set("Count", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		return cell((int)genv->get_stack(stack)->get_count());
	}));

	//Read top card
	set("Value", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		Solisp::Card *card = genv->get_stack(stack)->get_card();
		if(card != NULL)
			return cell(card->get_data().value);
		return cell(0);
	}));
	set("Suit", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		Solisp::Card *card = genv->get_stack(stack)->get_card();
		if(card != NULL)
			return cell(card->get_data().suit);
		return cell(0);
	}));
	set("Color", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		Solisp::Card *card = genv->get_stack(stack)->get_card();
		if(card != NULL) {
			char suit = card->get_data().suit;
			if(suit == 'H' || suit == 'D')
				return cell('R');
			else if(suit == 'C' || suit == 'S')
				return cell('B');
		}
		return cell(0);
	}));

	//Check if top card is hidden
	set("Hidden", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		if(genv->get_stack(stack)->get_count() > 0)
			return cell(genv->get_stack(stack)->get_card()->is_hidden());
		return cell(0);
	}));

	set("Matches", cell([](Enviroment *env, marker pos, marker end) {
		int from = env->num_eval(*pos++);
		int to = env->num_eval(*pos++);
		int count = 1;

		//If number included
		if(pos != end) {
			count = from;
			from = to;
			to = env->num_eval(*pos++);
		}

		DONE;
		return cell(genv->check_move(count, from, to));
	}));
}