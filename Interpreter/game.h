#include <vector>
#include <queue>

//#include "core/reference.h"

#include "layout.h"
#include "move.h"

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

#define MAXSTACKCOUNT 20

class Game {
private:
	//Game definition
	//Feature functions;
	char STACKCOUNT = 10;

	//Current game state
	Stack stack[MAXSTACKCOUNT];
	Move *current;

	//Current move
	char from = -1;
	char to = -1;
	int count = 0;

	//Apply current moves to stack array
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
		Card *destination = stack[to].getCard();
		Card *source = stack[from].getCard();
		stack[to].setCard(source);

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
		stack[from].setCard(source->getNext());
		source->setNext(destination);
	}

	//Run functions and check win
	void update() {
		apply();
	}

public:

	//Pick up cards from stack
	bool grab(int num, char from) {
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
	bool place(char to) {
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
	void cancel() {
		to = -1;
		from = -1;
		count = -1;
	}

	void undo();
	void redo();

protected:
	/*static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("grab", "num", "from"), &Game::grab);
		ClassDB::bind_method(D_METHOD("place", "to"), &Game::place);
		ClassDB::bind_method(D_METHOD("cancel"), &Game::cencel);
	}*/
};
