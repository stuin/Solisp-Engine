#include "game.h"

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
	while(current->getNext()->getTag(2)) {
		apply(current->getNext(), false);
		current = current->getNext();
	}
}

//Apply single card move
void Game::apply(Move *move, bool reverse) {
	int from = move->getFrom();
	int to = move->getTo();
	int count = move->getCount();
	bool flip = move->getTag(1);

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
	while(count > 1) {
		if(flip)
			source->flip();

		count--;
		source->setSlot(to);
		source = source->getNext();
	}

	//Disconnect from stack
	stack[from].setCard(source->getNext());

	//Reverse cards properly
	if(flip) {
		source->flip();
		source->setNext(NULL);
		stack[to].getCard()->reverse();
		stack[to].getCard()->setNext(destination);
		stack[to].setCard(source);
	} else
		source->setNext(destination);
}

//Run functions and check win
void Game::update() {
	apply();
}

//Call all setup functions
Card *Game::setup(Builder *builder) {
	STACKCOUNT = builder->setStacks(stack);

	return stack[0].getCard();
}

//Pick up cards from stack
bool Game::grab(int num, int from) {
	this->to = -1;
	this->from = -1;
	this->count = -1;

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

//Place cards from selected hand
bool Game::place(int to) {
	//Check if cards are in hand
	if(this->from == -1 || this->count == -1 || this->count != -1 || to > STACKCOUNT) {
		cancel();
		return false;
	}

	//Check for proper move
	if(stack[to].matches(count, stack[from].getCard())) {
		current->addNext(new Move(count, from, to, true, false, current));

		//Set hand
		this->to = to;
		this->count = -1;

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
}