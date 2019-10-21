namespace Solisp {
	class Move;
}

#include <bitset>

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

#define MOVETAGCOUNT 4

class Solisp::Move {
private:
	//Card movements
	int count;
	int from;
	int to;
	bitset<MOVETAGCOUNT> tags; //player, flip, valid, loop

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
		tags[0] = player;
		tags[1] = flip;
		tags[2] = true;

		//Link to previous
		this->last = last;
		//tags[3] = checkLoop(last);
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
		if(!tags[2])
			return;

		//If next move is not current
		if(next == NULL || !next->tags[2]) {
			clearForward();
			next = other;
		} else
			*next += other;
	}

	void undo();
	void redo();

	void correctCount(int count) {
		if(count < this->count)
			this->count = count;
	}

	//Get tag value
	bool getTag(int tag) {
		if(tag > 0 && tag < MOVETAGCOUNT)
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
