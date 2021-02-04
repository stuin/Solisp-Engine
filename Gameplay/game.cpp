#include "game.h"
#include "../Lisp/game_env.h"

#include <iostream>
#include <cstring>
#include <exception>

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Game rules and management.
 */

using Solisp::Game;

GameEnviroment game_env;

//Apply current moves to stack array
void Game::update() {
	//Recall invalid moves
	while(!current->get_tag(VALID)) {
		apply(current, true);
		current = current->get_last();
	}

	//Apply new moves
	while(current->get_next() != NULL && current->get_next()->get_tag(VALID)
			&& (stage != PLAYING || cardsLeft > 0)) {
		Move *move = current->get_next();
		if(move->get_tag(SOFT)) {
			if(grab(move->get_count(), move->get_from(), 1) && test(move->get_to(), 1)) {
				cout << "Successful soft move: "<< move->get_id() << "\n";
				move->soft_validate();
				current = move;
				apply(current, false);
			} else {
				move->soft_invalidate();
				//delete move;
			}
		} else {
			current = move;
			apply(current, false);

			if(stage == STARTING)
				current->mark_setup();
		}
	}

	//Check for game start functions
	if(stage == STARTING) {
		for(unc i = 1; i < STACKCOUNT; i++) {
			cell c = stack[i].get_function(ONSTART);
			if(c.type == EXPR)
				game_env.run(c, i, -1, current);
		}
		stage = PLAYING;
		update();
	} else if(stage == LOADING)
		stage = PLAYING;
}

//Apply single card move
void Game::apply(Move *move, bool reverse) {
	unc from = move->get_from();
	unc to = move->get_to();
	unsigned int count = move->get_count();
	bool flip = move->get_tag(FLIP);

	unsigned int realCount = 1;

	//Swap destinations for undo
	if(reverse) {
		from = to;
		to = move->get_from();
	}

	//Get card stacks
	Card *destination = stack[to].get_card();
	Card *source = stack[from].get_card();
	stack[to].set_card(source);

	if(source == NULL) {
		std::cerr << "Taking cards from empty stack\n";
	} else {
		//Find bottom moved card
		while(count > 1 && source->get_next() != NULL) {
			if(flip)
				source->flip();

			source->set_slot(to);
			source = source->get_next();

			count--;
			realCount++;
		}
		source->set_slot(to);

		//Update card counts
		if(to == from)
			destination = source->get_next();
		else {
			stack[from].set_card(source->get_next());
			stack[from].add_count(-realCount);
			stack[to].add_count(realCount);
		}

		//Record proper card count
		if(count > 1)
			move->correct_count(realCount);

		//Update win counter
		if((from == 0 || stack[from].get_tag(GOAL)) && !stack[to].get_tag(GOAL))
			cardsLeft += realCount;
		else if(!stack[from].get_tag(GOAL) && stack[to].get_tag(GOAL)) {
			cardsLeft -= realCount;
			if(cardsLeft <= 0)
				cout << "YOU WIN!!\n";
		}

		//Reverse cards properly
		if(flip) {
			source->flip();
			source->set_next(NULL);
			stack[to].get_card()->reverse(stack[to].get_count() - realCount);
			stack[to].get_card()->set_next(destination);
			stack[to].set_card(source);
		} else {
			source->set_next(destination);
			stack[to].get_card()->set_index(realCount);
		}
	}

	//Check for additional functions and moves
	if(move->get_user() > 0 && !reverse && stage != LOADING) {
		//Check stack grab function
		cell c = stack[from].get_function(ONGRAB);
		if(c.type == EXPR)
			game_env.run(c, from, to, move);

		//Check stack place function
		c = stack[to].get_function(ONPLACE);
		if(c.type == EXPR)
			game_env.run(c, to, from, move);
	}
}

//Deal out cards to starting positions
void Game::deal() {
	int remaining = 0;
	unc overflowSlot = 0;

	//Initial check of slots
	for(unc i = 1; i < STACKCOUNT; i++) {
		if(stack[i].start_hidden == (unsigned int)-1)
			overflowSlot = i;
		else
			remaining += stack[i].start_hidden + stack[i].start_shown;
	}

	//Loop until all placed
	while(remaining > 0) {
		//For each slot
		for(unc j = 1; j < STACKCOUNT; j++) {
			if(j != overflowSlot && stack[j].start_hidden + stack[j].start_shown > 0) {
				remaining--;

				if(stack[j].start_hidden > 0) {
					stack[j].start_hidden--;
					*current += new Move(0, j, 1, 0, false);
				} else if(stack[j].start_shown > 0) {
					stack[j].start_shown--;
					*current += new Move(0, j, 1, 0, true);
				}
			}
		}
	}

	//Move remaining cards to overflow
	if(overflowSlot != 0)
		*current += new Move(0, overflowSlot, -1, 0, false);
}

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder, Move *saved) {
	if(users != NULL || current != NULL)
		clear();

	//Read rules file
	struct setup output = builder->build_ruleset(stack);
	if(output.count == 0)
		return NULL;

	//Build game structures
	STACKCOUNT = output.count;
	stack[0].set_card(output.deck);
	game_env.setup(stack, STACKCOUNT, [&]() { update(); });
	users = (Hand*)malloc(3 * sizeof(Hand));

	//Check for game variables
	std::ifstream *rule_file = output.file;
	if(!rule_file->eof()) {
		game_env.shift_env(true);
		game_env.read_stream(*rule_file, EXPR);
	}

	//Start game history
	if(saved == NULL) {
		current = new Move(0, 0, output.seed, false, false);
		deal();
		stage = STARTING;
	} else {
		current = saved;
		stage = LOADING;
	}

	return stack[0].get_card();
}

//Delete all data specific to game
void Game::clear() {
	for(int i = 0; i < STACKCOUNT; ++i)
		stack[i] = Stack();

	STACKCOUNT = 0;
	players = 2;
	stage = NONE;
	cardsLeft = 0;

	//stack.clear();
	if(current != NULL) {
		current->clear_forward();
		delete current;
		free(users);
	}

	current = NULL;
	users = NULL;
}

//Pick up cards from stack
bool Game::grab(unsigned int num, unc from, unc user) {
	users[user] = { 0, 0, 0, 0 };

	if(from > STACKCOUNT || from == 0)
		return false;

	//Check if stack is button
	if(stack[from].get_tag(BUTTON) && user > 1) {
		*current += new Move(from, from, 0, user, false);
		update();
		return false;
	}

	//Fail if stack empty or marked as output
	if(stack[from].get_card() == NULL || stack[from].get_tag(OUTPUT) || num < 1)
		return false;

	//Flip one card if top hidden
	if(stack[from].get_card()->is_hidden() && user > 1) {
		*current += new Move(from, from, 1, user, true);
		update();
		return false;
	}

	//Check for null or hidden card in stack
	unsigned int i = 1;
	Card *card = stack[from].get_card();
	while(i < num && card != NULL) {
	 	card = card->get_next();
		if(card == NULL || card->is_hidden())
			return false;
		i++;
	}

	//Check if stack has function defined
	cell c = stack[from].get_function(GRABIF);
	if(c.type == EXPR && !game_env.run(c, from, -1, current))
		return false;

	//Set picked cards
	if(stack[0].matches(num, stack[from].get_card())) {
		users[user] = { from, 0, 0, num };
		return true;
	}

	return false;
}

//Test placement of cards from selected slot
bool Game::test(unc to, unc user) {
	//Check if position and users is valid
	if(to > STACKCOUNT || to == 0 || users[user].from == 0 || users[user].count == 0) {
		cancel(user);
		return false;
	}

	//Check for proper move
	unc from = users[user].from;
	if(to == from || stack[to].matches(users[user].count, stack[from].get_card())) {
		//Check if stack has function defined
		cell c = stack[to].get_function(PLACEIF);
		if(c.type == EXPR && !game_env.run(c, to, from, current))
			return false;

		users[user].tested = to;
		return true;
	}

	users[user].tested = 0;
	return false;
}

bool Game::place(unc to, unc user) {
	unc from = users[user].from;

	//Check for proper move
	if(to != from && (users[user].tested == to || test(to, user))) {
		*current += new Move(from, to, users[user].count, user, false);

		update();
		cancel(user);
		return true;
	}

	//Check for valid swap
	if(to != from && stack[to].get_tag(SWAP) && stack[from].get_tag(SWAP)
		&& stack[from].get_count() == users[user].count) {
		*current += new Move(0, 0, 0, user, false);
		*current += new Move(from, 0, stack[from].get_count(), 0, false);
		*current += new Move(to, from, stack[to].get_count(), 0, false);
		*current += new Move(0, to, stack[from].get_count(), 0, false);

		update();
		cancel(user);
		return true;
	}

	cancel(user);
	return false;
}

//Clear hand
void Game::cancel(unc user) {
	users[user] = { 0, 0, 0, 0 };
}

//Move history manipulation
void Game::undo(unc user) {
	cancel(user);
	current->undo();
}
void Game::redo(unc user) {
	cancel(user);

	if(current->get_next() != NULL)
		current->redo();
}

//Save to file
void Game::save(string file) {
	//Get starting move
	Move *first = current;
	int count = 0;
	while(first->get_last() != NULL) {
		first = first->get_last();
	}

	//Open output file
	FILE *outfile = fopen(file.c_str(), "wb");
	size_t size = sizeof(struct MovePacket);

	//struct MovePacket *data = first->get_data();
	//printf("%2X %2X %2X %X %X %2X\n", data->from, data->to, data->user, data->count, data->id, data->tags);

	//Save each move
	while(first != NULL && first->get_tag(VALID)) {
		fwrite(first->get_data(), size, 1, outfile);
		first = first->get_next();
		++count;
	}

	cout << size << ":" << count << "\n";
	fclose(outfile);
}

//Load game from file
void Game::load(string file, string rule_file) {
	//Open file
	FILE *infile = fopen(file.c_str(), "rb");
	if(infile == NULL) {
		cout << "Error, unreadable save file\n";
		return;
	}

	struct MovePacket data;
	size_t size = sizeof(struct MovePacket);

	//Read first move
	fread(&data, size, 1, infile);
	Move *first = new Move(data);
	Move *added = first;
	unsigned int seed = first->get_count();

	//Read all other moves
	int count = 1;
	while(fread(&data, size, 1, infile)) {
		*added += new Move(data);
		added = added->get_next();
		++count;
	}

	cout << count << " moves loaded with seed " << seed << "\n";

	//Set game
	Solisp::Builder builder(rule_file, seed);
	setup(&builder, first);
	update();
}
