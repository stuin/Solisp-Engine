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
void Game::apply(Move move, bool reverse) {
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
		moves.pop_back();
	} else {
		moves.push_back(move);
	}

	if(stack[from].get_count() == 0 && count > 0) {
		std::cerr << "Taking cards from empty stack " << (int)from << "\n";
		return;
	} else if(count > 0) {
		//Get card stacks
		Card *destination = stack[to].get_card();
		Card *source = stack[from].get_card();
		stack[to].set_card(source);

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
			move.count = realCount;

		//Update win counter
		if((from == 0 || stack[from].get_tag(GOAL)) && !stack[to].get_tag(GOAL))
			cardsLeft += realCount;
		else if(!stack[from].get_tag(GOAL) && stack[to].get_tag(GOAL)) {
			cardsLeft -= realCount;
			if(cardsLeft <= 0) {
				cout << "YOU WIN!!\n";
				cout << moves.size() << " moves recorded\n";
			}
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
	if(move.user > 0 && !reverse && stage != LOADING) {
		//Check stack place function
		stack[from].run_function(ONGRAB, from, to);
		stack[to].run_function(ONPLACE, to, from);
	}
}

//Public apply methods
void Game::apply(Move move, unsigned int server) {
	if(server == 0) {
		apply(move, false);
	} else if(!(moves[server] == move)) {
		while(server > moves.size() - 1)
			apply(moves.back(), true);
		apply(move, false);
	}
}
void Game::apply(unc from, unc to, unsigned int count, unc user, bool flip) {
	apply(Move(from, to, count, user, flip), false);
}

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder, bool saved) {
	if(users != NULL)
		clear();

	//Read rules file
	struct setup output = builder->build_ruleset(stack);
	if(output.count == 0)
		return NULL;
	//cout << output.deck->print_stack() << "\n";

	//Build game structures
	STACKCOUNT = output.count;
	stack[0].set_card(output.deck);
	stack[0].full_count();
	game_env.setup(stack, STACKCOUNT, [&](Move move) { apply(move); });
	moves.reserve(1000);

	//Setup user hands
	users = (Hand*)malloc(players * sizeof(Hand));
	for(int i = 0; i < players; i++)
		users[i] = {0,0,0,0};

	//Check for game variables
	std::ifstream *rule_file = output.file;
	if(!rule_file->eof()) {
		game_env.shift_env(true);
		game_env.read_stream(*rule_file, EXPR);
	}

	//Start game history
	if(!saved) {
		stage = LOADING;
		apply(0, 0, output.seed, 0, false);
		moves.back().set_tag(SETUP, true);
		cardsLeft = 0;
		deal();
		stage = STARTING;

		//Check for game start functions
		for(unc i = 1; i < STACKCOUNT; i++)
			stack[i].run_function(ONSTART, i, -1);

		stage = PLAYING;
	} else
		stage = LOADING;

	return stack[0].get_card();
}

//Deal out cards to starting positions
void Game::deal() {
	unsigned int remaining = 0;
	unc overflowSlot = 0;

	//Initial check of slots
	for(unc i = 1; i < STACKCOUNT; i++) {
		if(stack[i].start_hidden == -1)
			overflowSlot = i;
		else
			remaining += stack[i].start_hidden + stack[i].start_shown;
	}

	//Loop until all placed
	while(remaining > 0 && stack[0].get_count() > 0) {
		for(unc j = 1; j < STACKCOUNT; j++) {
			if(j != overflowSlot && stack[j].start_hidden + stack[j].start_shown > 0) {
				remaining--;
				if(stack[j].start_hidden-- > 0)
					apply(0, j, 1, 0, false);
				else if(stack[j].start_shown-- > 0)
					apply(0, j, 1, 0, true);
			}
		}
	}

	//Move remaining cards to overflow
	if(overflowSlot != 0)
		apply(0, overflowSlot, -1, 0, false);
	moves.back().set_tag(SETUP, true);
}

//Delete all data specific to game
void Game::clear() {
	for(int i = 0; i < STACKCOUNT; ++i)
		stack[i] = Stack();

	//General variables
	STACKCOUNT = 0;
	players = 3;
	stage = NONE;
	cardsLeft = 0;

	//Clear history
	moves.clear();

	//Clear users
	if(users != NULL)
		free(users);
	users = NULL;
}

//Pick up cards from stack
bool Game::grab(unsigned int num, unc from, unc user) {
	users[user] = { 0, 0, 0, 0 };

	if(from > STACKCOUNT || from == 0)
		return false;

	//Check if stack is button
	if(stack[from].get_tag(BUTTON) && user > 0) {
		moves.push_back(Move(from, from, 0, user, false));

		//Check stack grab function
		stack[from].run_function(ONGRAB, from, from);
		return false;
	}

	//Fail if stack empty or marked as output
	if(stack[from].get_card() == NULL || stack[from].get_tag(OUTPUT) || num < 1)
		return false;

	//Flip one card if top hidden
	if(stack[from].get_card()->is_hidden() && user > 0) {
		apply(from, from, 1, user, true);
		return user == 1;
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
	if(stack[from].run_function(GRABIF, from, -1))
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
		if(stack[to].run_function(PLACEIF, to, from))
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
		&& stack[from].get_count() == users[user].count) {
		apply(0, 0, 0, user, false);
		apply(from, 0, stack[from].get_count(), 0, false);
		apply(to, from, stack[to].get_count(), 0, false);
		apply(0, to, stack[from].get_count(), 0, false);

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
void Game::undo(unc user, bool anyone) {
	cancel(user);

	//Find most recent user move
	unsigned int i = moves.size() - 1;
	while((moves[i].user == 0 || (anyone && moves[i].user != user)) && !moves[i].get_tag(SETUP))
		--i;

	//Enforce undo
	if(moves[i].user > 0 && !moves[i].get_tag(SETUP)) {
		while(moves.size() > i)
			apply(moves.back(), true);
	}
}

//Print out all cards in game
void Game::print_game() {
	int i = 0;
	while(i < STACKCOUNT)	{
		cout << "Stack " << i << " at (" <<
			stack[i].x << "," << stack[i].y << "): ";
		if(stack[i].get_card() != NULL)
			cout << stack[i].get_card()->print_stack();
		cout << "\n";
		i++;
	}
}

//Save to file
void Game::save(string file) {
	//Open output file
	FILE *outfile = fopen(file.c_str(), "wb");
	size_t size = sizeof(Move);

	if(outfile == NULL) {
		std::cerr << "Error: Could not open file " << file << "\n";
		return;
	}

	//struct MovePacket *data = first->get_data();
	//printf("%2X %2X %2X %X %X %2X\n", data->from, data->to, data->user, data->count, data->id, data->tags);

	//Save each move
	for(unsigned int i = 0; i < moves.size(); i++)
		fwrite(&(moves[i]), size, 1, outfile);

	cout << "Saved " << moves.size() << " moves\n"; 
	fclose(outfile);
}

//Load game from file
void Game::load(string file, string rule_file) {
	//Open file
	FILE *infile = fopen(file.c_str(), "rb");
	if(infile == NULL) {
		std::cerr << "Error, unreadable save file\n";
		return;
	}

	Move data;
	size_t size = sizeof(Move);

	//Read starting data
	fread(&data, size, 1, infile);
	unsigned int seed = data.count;
	apply(data);
	cardsLeft = 0;

	//Set game
	Solisp::Builder builder(rule_file, seed);
	setup(&builder, true);
	cout << "Using seed " << seed << "\n";

	//Read all moves
	while(fread(&data, size, 1, infile))
		apply(data);

	stage = PLAYING;
	cout << "Loaded " << moves.size() << " moves\n";
}

//Full copy constructor
Game::Game(const Game &other) {
	//Copy simple variables
	STACKCOUNT = other.STACKCOUNT;
	stage = other.stage;
	cardsLeft = other.cardsLeft;
	players = other.players;
	moves.clear();

	//Copy complex state
	for(int i = 0; i < STACKCOUNT; i++)
		stack[i] = Stack(other.stack[i]);
	for(int i = 0; i < other.moves.size(); i++)
		moves.push_back(other.moves[i]);

	//Clear users
	if(users != NULL)
		free(users);
	users = (Hand*)malloc(players * sizeof(Hand));
}

Stack::Stack() {
	cell *c = new cell(0);
	for(int i = 0; i < STACKFUNCOUNT; i++)
		functions[i] = c;
}

void Stack::set_function(void *function, func_tag type) {
	sexpr *func = (sexpr *)function;
	(*func)[0] = cell("+");
	functions[type] = (void *)(new cell(*func, EXPR));
}

bool Stack::run_function(func_tag func, unsigned char first, unsigned char second) {
	cell c = *((cell *)(functions[func]));
	return c.type == EXPR && game_env.run(c, first, second);
}
