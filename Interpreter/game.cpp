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
		std::cout << "Move " << current->getNext()->getCount() << " to " << current->getNext()->getTo();
		apply(current->getNext(), false);
		current = current->getNext();
		std::cout << " Flip: " << current->getNext()->getTag(1) << "\n";
	}
	std::cout << "Done\n";
}

//Apply single card move
void Game::apply(Move *move, bool reverse) {
	std::cout << " Start";

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

	std::cout << " Loop";

	//Find bottom moved card
	/*while(count > 1 && source->getNext() != NULL) {
		if(flip)
			source->flip();

		source->setSlot(to);
		source = source->getNext();

		count--;
		realCount++;
	}*/

	//Disconnect from stack
	stack[from].setCard(source->getNext());
	stack[from].addCount(-realCount);
	stack[to].addCount(realCount);

	//Record proper card count
	//if(count > 1)
	//	move->correctCount(realCount);

	//Reverse cards properly
	if(flip) {
		source->flip();
		source->setNext(NULL);
		stack[to].getCard()->reverse(stack[to].getCount() - realCount);
		stack[to].getCard()->setNext(destination);
		stack[to].setCard(source);
	} else {
		source->setNext(destination);
		//source->setIndex(realCount);
	}
}

//Run functions and check win
void Game::update() {
	apply();
}

//Deal out cards to starting positions
void Game::deal() {
	int remaining = 0;
	int lastSlot = -1;
	int overflowSlot = -1;

	//Initial check of slots
	for(int i = 1; i < STACKCOUNT; i++) {
		if(stack[i].start_hidden == -1)
			overflowSlot = i;
		else {
			if(stack[i].start_hidden + stack[i].start_shown > 0) {
				lastSlot = i;
				remaining += stack[i].start_hidden + stack[i].start_shown;
			}
		}
	}

	//Loop until all placed
	while(remaining > 8) {
		std::cout << remaining << " cards left\n";

		//For each slot
		for(int j = 1; j < lastSlot; j++) {
			if(j != overflowSlot && stack[j].start_hidden + stack[j].start_shown > 0) {
				remaining--;

				if(stack[j].start_hidden > 0) {
					stack[j].start_hidden--;
					*current += new Move(1, 0, j, false, false, current);
				} else if(stack[j].start_shown > 0){
					stack[j].start_shown--;
					*current += new Move(1, 0, j, false, true, current);
				}
			}
		}
	}

	//Move remaining cards to overflow
	if(overflowSlot != -1)
		*current += new Move(1, 0, 0, false, false, current);
}

//Call all setup functions
Solisp::Card *Game::setup(Builder *builder) {
	STACKCOUNT = builder->setStacks(stack);
	stack[0].setCard(builder->getDeck());

	deal();
	apply();

	return stack[0].getCard();
}

//Pick up cards from stack
bool Game::grab(int num, int from) {
	this->to = -1;
	this->from = -1;
	this->count = -1;
	this->tested = -1;

	if(from > STACKCOUNT)
		return false;

	//If button and top hidden
	if(stack[from].getTag(4) && stack[from].getCard()->isHidden()) {
		this->from = from;
		return true;
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
	if(this->from == -1 || this->count == -1 || this->count != -1 || to > STACKCOUNT) {
		cancel();
		return false;
	}

	//Check for proper move
	if(stack[to].matches(count, stack[from].getCard())) {
		tested = to;
		return true;
	}
	
	tested = -1;
	return true;
}

bool Game::place(int to) {
	//Check for proper move
	if(tested == to || test(to)) {
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