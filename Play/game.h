#include <vector>
#include <queue>

#include "structures.h"

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

class Game {
private:
	//Current game state
	Slot slot[];
	Move *current;
	bool ended;

	//Game definition
	Feature functions;
	char SLOTCOUNT;

	//Current move
	char from = -1;
	char to = -1;
	int count = 0;

	//Apply current moves to slot array
	void apply() {
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
	void apply(Move *move, bool reverse) {
		char from = move->getFrom();
		char to = move->getTo();
		int count = move->getCount();
		bool flip = move->getTag(1);

		//Swap destinations for undo
		if(reverse) {
			from = to;
			to = move->getFrom();
		}

		//Get card stacks
		Card *destination = slot[to].getCard();
		Card *source = slot[from].getCard();
		slot[to].setCard(source);

		//Find bottom moved card
		while(count > 1) {
			if(flip)
				source->flip();

			count--;
			source = source->getNext();
		}

		if(flip)
			source->flip();

		//Set final cards
		slot[from].setCard(source->getNext());
		source.setNext(destination);
	}

	//Run functions and check win
	void update() {
		apply();
	}

public:
	//Pick up cards from slot
	bool grab(int num, char from) {
		this->to = -1;
		this->from = -1;
		this->count = -1;

		if(from > SLOTCOUNT)
			return false;

		//If button and top hidden
		if(slot[from].getTag(4) && getCard()->isHidden()) {
			this->from = from;
			return true;
		}

		//If slot is output
		if(slot[from].getTag(2) || (slot[from].getTag(3) && num > 1) || num < 1)
			return false;

		//Check for null or hidden card in stack
		int i = 0;
		Card *card = slot[from].getCard();
		while(i < count) {
			if(card == NULL || card->isHidden())
				return false;
			i++;
			card = card->getNext();
		}

		//Set picked cards
		if(slot[0].matches(num, slot[from].getCard())) {
			this->from = from;
			this->count = num;
			return true;
		}

		return false;
	}

	//Place cards from selected hand
	bool place(char to) {
		//Check if cards are in hand
		if(this->from == -1 || this->count == -1 || this->count != -1 || to > SLOTCOUNT) {
			cancel();
			return false;
		}

		//Check for proper move
		if(slot[to].matches(count, slot[from].getCard())) {
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
	void cancel() {
		to = -1;
		from = -1;
		count = -1;
	}

	void undo();
	void redo();

	//Check game state
	bool ended();
}
