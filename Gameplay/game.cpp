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

//Apply single card move
void Game::apply(bool reverse) {
	Move move = moves.back();

	//Initial variables
	unc from = move.from;
	unc to = move.to;
	unsigned int count = move.count;
	bool flip = move.get_tag(FLIP);
	unsigned int realCount = 1;

	//Swap destinations for undo
	if(reverse) {
		from = to;
		to = move.from;

		//Delete undo move
		moves.pop_back();
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
		if(!reverse && count > 1 && realCount < move.count)
			moves.back().count = realCount;

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
			stack[to].get_card()->reverse(stack[to].count - realCount);
			stack[to].get_card()->set_next(destination);
			stack[to].set_card(source);
		} else {
			source->set_next(destination);
			stack[to].get_card()->set_index(realCount);
		}
	}

	//Check for additional functions and moves
	if(move.user > 0 && !reverse && stage != LOADING) {
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

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder, Move *saved) {
	if(users != NULL)
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
	moves.reserve(1000);

	//Check for game variables
	std::ifstream *rule_file = output.file;
	if(!rule_file->eof()) {
		game_env.shift_env(true);
		game_env.read_stream(*rule_file, EXPR);
	}

	//Start game history
	if(saved == NULL) {
		moves.emplace_back(0, 0, output.seed, 0, false);
		moves.back().set_tag(SETUP, true);
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

	//General variables
	STACKCOUNT = 0;
	players = 2;
	stage = NONE;
	cardsLeft = 0;

	//Clear history
	moves.clear();
	server = 0;

	//Clear users
	if(users != NULL)
		free(users);
	users = NULL;
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
					apply(0, j, 1, 0, false);
				} else if(stack[j].start_shown > 0) {
					stack[j].start_shown--;
					apply(0, j, 1, 0, true);
				}
			}
		}
	}

	//Move remaining cards to overflow
	if(overflowSlot != 0)
		apply(0, overflowSlot, -1, 0, false);
	moves.back().set_tag(SETUP, true);

	//Check for game start functions
	for(unc i = 1; i < STACKCOUNT; i++) {
		cell c = stack[i].get_function(ONSTART);
		if(c.type == EXPR)
			game_env.run(c, i, -1, current);
	}
	moves.back().set_tag(SETUP, true);
	stage = PLAYING;
}

//Pick up cards from stack
bool Game::grab(unsigned int num, unc from, unc user) {
	users[user] = { 0, 0, 0, 0 };

	if(from > STACKCOUNT || from == 0)
		return false;

	//Check if stack is button
	if(stack[from].get_tag(BUTTON) && user > 1) {
		apply(from, from, 0, user, false);
		return false;
	}

	//Fail if stack empty or marked as output
	if(stack[from].get_card() == NULL || stack[from].get_tag(OUTPUT) || num < 1)
		return false;

	//Flip one card if top hidden
	if(stack[from].get_card()->is_hidden() && user > 1) {
		apply(from, from, 1, user, true);
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
		apply(from, to, users[user].count, user, false);
		cancel(user);
		return true;
	}

	//Check for valid swap
	if(to != from && stack[to].get_tag(SWAP) && stack[from].get_tag(SWAP)
		&& stack[from].count == users[user].count) {
		apply(0, 0, 0, user, false);
		apply(from, 0, stack[from].count, 0, false);
		apply(to, from, stack[to].count, 0, false);
		apply(0, to, stack[from].count, 0, false);

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

//Undo move in history
void Game::undo(unc user) {
	cancel(user);

	//Find most recent user move
	unsigned int i = moves.size() - 1;
	while(moves[i].user == 0 && !moves[i].get_tag(SETUP))
		--i;

	//Enforce undo
	if(moves[i].user == user) {
		while(moves.size() - 1 > i)
			apply(true);
	}
}

//Public apply methods
void Game::apply(Move move) {
	moves.push_back(move);
	apply(false);
}
void Game::apply(unc from, unc to, unsigned int count, unc user, bool flip) {
	moves.emplace_back(from, to, count, user, flip);
	apply(false);
}

//Save to file
void Game::save(string file) {
	//Open output file
	FILE *outfile = fopen(file.c_str(), "wb");
	size_t size = sizeof(Move);

	//struct MovePacket *data = first->get_data();
	//printf("%2X %2X %2X %X %X %2X\n", data->from, data->to, data->user, data->count, data->id, data->tags);

	//Save each move
	for(Move *move : moves)
		fwrite(move, size, 1, outfile);

	cout << size << ": " << moves.size() << "\n";
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

	struct Move data;
	size_t size = sizeof(struct Move);

	//Read all moves
	while(fread(&data, size, 1, infile))
		apply(data);
	unsigned int seed = moves.front().count;

	cout << moves.size() << " moves loaded with seed " << seed << "\n";

	//Set game
	Solisp::Builder builder(rule_file, seed);
	setup(&builder, first);
	update();
}
