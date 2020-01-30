#include "game.h"

#include <iostream>
#include <exception>

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Game rules and management.
 */

using Solisp::Game;

//Apply current moves to stack array
void Game::update() {
	//Recall invalid moves
	while(!current->get_tag(VALID)) {
		apply(current, true);
		current = current->get_last();
	}

	//Apply new moves
	while(current->get_next() != NULL && current->get_next()->get_tag(VALID)) {
		apply(current->get_next(), false);
		current = current->get_next();
	}

	//Check for game start functions
	if(!started) {
		for(int i = 1; i < STACKCOUNT; i++) {
			cell c = stack[i].get_function(ONSTART);
			if(c.type == EXPR)
				env.run(c, i, current);
		}
		started = true;
		update();
	}
}

//Apply single card move
void Game::apply(Move *move, bool reverse) {
	int from = move->get_from();
	int to = move->get_to();
	int count = move->get_count();
	bool flip = move->get_tag(FLIP);

	int realCount = 1;

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
	if(move->get_tag(PLAYER) && !reverse) {
		//Check stack grab function
		cell c = stack[from].get_function(ONGRAB);
		if(c.type == EXPR) {
			cout << "Running " << env.str_eval(c, true) << "on stack " << from << "\n";
			env.run(c, from, move);
		}

		//Check stack place function
		c = stack[to].get_function(ONPLACE);
		if(c.type == EXPR)
			env.run(c, to, move);
	}
}

//Deal out cards to starting positions
void Game::deal() {
	int remaining = 0;
	int overflowSlot = -1;

	//Initial check of slots
	for(int i = 1; i < STACKCOUNT; i++) {
		if(stack[i].start_hidden == -1)
			overflowSlot = i;
		else
			remaining += stack[i].start_hidden + stack[i].start_shown;
	}
	//std::cout << "Overflow slot: " << overflowSlot << "\n";

	//Loop until all placed
	while(remaining > 0) {
		//For each slot
		for(int j = 1; j < STACKCOUNT; j++) {
			if(j != overflowSlot && stack[j].start_hidden + stack[j].start_shown > 0) {
				remaining--;

				if(stack[j].start_hidden > 0) {
					stack[j].start_hidden--;
					*current += new Move(1, 0, j, false, false, current);
				} else if(stack[j].start_shown > 0) {
					stack[j].start_shown--;
					*current += new Move(1, 0, j, false, true, current);
				}
			}
		}
		//std::cout << remaining << " cards left\n";
	}

	//Move remaining cards to overflow
	if(overflowSlot != -1)
		*current += new Move(1000, 0, overflowSlot, false, false, current);
}

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder) {
	srand(time(NULL));
	stack[0].set_card(builder->get_deck());
	Card *card = stack[0].get_card();

	STACKCOUNT = builder->set_stacks(stack);
	env.setup(stack, STACKCOUNT);

	deal();

	return card;
}

//Pick up cards from stack
bool Game::grab(int num, int from) {
	this->to = -1;
	this->from = -1;
	this->count = -1;
	this->tested = -1;

	if(from > STACKCOUNT)
		return false;

	//Check if stack is button
	if(stack[from].get_tag(BUTTON)) {
		*current += new Move(0, from, from, true, false, current);
		update();
		return false;
	}

	//Fail if stack marked as output
	if(stack[from].get_tag(OUTPUT) || num < 1)
		return false;

	//Flip one card if top hidden
	if(stack[from].get_card()->is_hidden()) {
		*current += new Move(1, from, from, true, true, current);
		update();
		return false;
	}

	//Check for null or hidden card in stack
	int i = 1;
	Card *card = stack[from].get_card();
	while(i < num && card != NULL) {
	 	card = card->get_next();
		if(card == NULL || card->is_hidden())
			return false;
		i++;
	}

	//Check if stack has function defined
	cell c = stack[from].get_function(GRABIF);
	if(c.type == EXPR && !env.run(c, from, current))
		return false;

	//Set picked cards
	if(stack[0].matches(num, stack[from].get_card())) {
		this->from = from;
		this->count = num;
		return true;
	}

	return false;
}

//Test placement of cards from selected hand
bool Game::test(int to) {
	//Check if cards are in hand
	if(this->from == -1 || this->count == -1 || this->to != -1 || to > STACKCOUNT) {
		cancel();
		return false;
	}

	//Check for proper move
	if(to == from || stack[to].matches(count, stack[from].get_card())) {
		//Check if stack has function defined
		cell c = stack[to].get_function(PLACEIF);
		if(c.type == EXPR && !env.run(c, to, current))
			return false;

		tested = to;
		return true;
	}

	tested = -1;
	return false;
}

bool Game::place(int to) {
	//Check for proper move
	if(to != from && (tested == to || test(to))) {
		*current += new Move(count, from, to, true, false, current);

		//Set hand
		this->to = to;
		this->count = -1;
		tested = -1;

		update();
		return true;
	}

	cancel();
	return false;
}

//Clear hand
void Game::cancel() {
	to = -1;
	from = -1;
	count = -1;
	tested = -1;
}

//Move history manipulation
void Game::undo() {
	current->undo();
}
void Game::redo() {
	current->redo();
}
