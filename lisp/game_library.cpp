#include "game_env.h"

/*
 * Created by Stuart Irwin on 28/1/2020.
 * Lisp structures for moving cards during game
 */

cell GameEnviroment::general_move(int num, bool flip, bool player) {
	return cell([num, flip, player](Enviroment *env, marker pos, marker end) {
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
		if(genv->both_valid(from, to))
			genv->add_move(count, from, to, flip, player);
		return cell(1);
	});
}

cell GameEnviroment::soft_move(int num) {
	return cell([num](Enviroment *env, marker pos, marker end) {
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
		if(genv->both_valid(from, to) && genv->test_move(count, from, to))
			genv->add_move(count, from, to, false, true);
		return cell(1);
	});
}

void GameEnviroment::build_library_game() {
	//Base moves
	set("Move", general_move(1, false, false));
	set("Move-All", general_move(1000, false, false));

	//Flip moves
	set("Flip-Move", general_move(1, true, false));
	set("Flip-Move-All", general_move(1000, true, false));

	//Soft moves
	set("Soft-Move", soft_move(1));
	set("Soft-Move-All", soft_move(1000));

	//Count cards in stack
	set("Count", cell([](Enviroment *env, marker pos, marker end) {
		int stack = env->num_eval(*pos++);
		DONE;
		return cell(genv->get_count(stack));
	}));
}