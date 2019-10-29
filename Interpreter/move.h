namespace Solisp {
	class Move;
}

#include <bitset>

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

#define MOVETAGCOUNT 4

enum move_tags { PLAYER, FLIP, VALID, LOOP };

class Solisp::Move {
private:
	//Card movements
	int count;
	int from;
	int to;
	bitset<MOVETAGCOUNT> tags;

	//List references
	Move *next = NULL;
	Move *last;

	//Check for circular card movement
	bool checkLoop(Move *other) {
		if(this->from == other->from || this->to == other->to || other->from == 0)
			return false;

		if(this->from == other->to && this->to == other->from)
			return this->count == other->count;

		return checkLoop(other->last);
	}

public:
	//Build new move
	Move(int count, int from, int to, bool player, bool flip, Move *last) {
		//Set general move
		this->count = count;
		this->from = from;
		this->to = to;

		//Set special tags
		tags[PLAYER] = player;
		tags[FLIP] = flip;
		tags[VALID] = true;

		//Link to previous
		this->last = last;
		//tags[LOOP] = checkLoop(last);
	}

	//Recursizely delete backward
	~Move() {
		delete last;
	}

	//Recursizely delete forward
	void clearForward() {
		if(next != NULL) {
			next->last = NULL;
			next->clearForward();
			delete next;
		}
	}

	//Add new move to history
	void operator+=(Move *other) {
		//Check if state is valid
		if(!tags[VALID])
			return;

		//If next move is not current
		if(next == NULL || !next->tags[VALID]) {
			clearForward();
			next = other;
		} else
			*next += other;
	}

	//Set move to invalid
	void undo() {
		tags[VALID] = false;
		if(!tags[PLAYER] && from != 0 && last != NULL)
			last->undo();
	}

	//Revalidate next move
	void redo(bool first=true) {
		if(tags[VALID] && next != NULL)
			next->redo(true);
		else if(first || !tags[PLAYER]) {
			tags[VALID] = true;
			if(next != NULL)
				next->redo(false);
		}
	}

	//Set actual card count if move-all used
	void correctCount(int count) {
		if(count < this->count)
			this->count = count;
	}

	//Get tag value
	bool getTag(int tag) {
		if(tag >= 0 && tag < MOVETAGCOUNT)
			return tags[tag];
		return false;
	}

	//Normal getters
	int getCount() {
		return count;
	}

	int getFrom() {
		return from;
	}

	int getTo() {
		return to;
	}

	Move *getNext() {
		return next;
	}

	Move *getLast() {
		return last;
	}
};
