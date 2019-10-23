#include "game.h"

#include <iostream>
#include <exception>

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Game rules and management.
 */

using Solisp::Game;

//Apply current moves to stack array
void Game::apply() {
	//Recall invalid moves
	while(!current->getTag(2)) {
		apply(current, true);
		current = current->getLast();
	}

	//Apply new moves
	while(current->getNext() != NULL && current->getNext()->getTag(2)) {
		apply(current->getNext(), false);
		current = current->getNext();
	}
	std::cout << "Done\n";
}

//Apply single card move
void Game::apply(Move *move, bool reverse) {
	int from = move->getFrom();
	int to = move->getTo();
	int count = move->getCount();
	bool flip = move->getTag(1);

	int realCount = 1;

	//Swap destinations for undo
	if(reverse) {
		from = to;
		to = move->getFrom();
	}

	//Get card stacks
	Card *destination = stack[to].getCard();
	Card *source = stack[from].getCard();
	stack[to].setCard(source);

	//Find bottom moved card
	while(count > 1 && source->getNext() != NULL) {
		if(flip)
			source->flip();

		source->setSlot(to);
		source = source->getNext();

		count--;
		realCount++;
	}
	source->setSlot(to);

	//Disconnect from stack
	stack[from].setCard(source->getNext());
	stack[from].addCount(-realCount);
	stack[to].addCount(realCount);

	//Record proper card count
	if(count > 1)
		move->correctCount(realCount);

	//Reverse cards properly
	if(flip) {
		source->flip();
		source->setNext(NULL);
		stack[to].getCard()->reverse(stack[to].getCount() - realCount);
		stack[to].getCard()->setNext(destination);
		stack[to].setCard(source);
	} else {
		source->setNext(destination);
		source->setIndex(realCount);
	}
}

//Run functions and check win
void Game::update() {
	apply();
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
		std::cout << remaining << " cards left\n";
	}

	//Move remaining cards to overflow
	if(overflowSlot != -1)
		*current += new Move(1000, 0, 1, false, false, current);
}

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder) {
	STACKCOUNT = builder->setStacks(stack);
	
	stack[0].setCard(builder->getDeck());
	Card *card = stack[0].getCard();

	deal();
	//apply();

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

	//If top card hidden
	if(stack[from].getCard()->isHidden()) {
		*current += new Move(1, from, from, true, true, current);
		this->from = from;
		return false;
	}

	//If stack is output
	if(stack[from].getTag(2) || (stack[from].getTag(3) && num > 1) || num < 1)
		return false;

	//Check for null or hidden card in stack
	int i = 0;
	Card *card = stack[from].getCard();
	while(i < count) {
		if(card == NULL || card->isHidden())
			return false;
		i++;
		card = card->getNext();
	}

	//Set picked cards
	if(stack[0].matches(num, stack[from].getCard())) {
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
	if(to == from || stack[to].matches(count, stack[from].getCard())) {
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
