#include <bitset>

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

#define MOVETAGCOUNT 4

class Move {
private:
	//Card movements
	int count;
	char from;
	char to;
	bitset<MOVETAGCOUNT> tags; //player, flip, valid, loop

	//List references
	Move *next = NULL;
	Move *last;

	//Check for circular card movement
	bool checkLoop(Move *other) {
		if(this->from == other->from || this->to == other->to)
			return false;

		if(this->from == other->to && this->to == other->from)
			return this->count == other->count;

		return checkLoop(other->last);
	}

public:
	//Build new move
	Move(int count, char from, char to, bool player, bool flip, Move *last) {
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

	//Add new move to history
	void addNext(Move *other) {
		//Check if state is valid
		if(!tags[2])
			return;

		//If next move is not current
		if(next == NULL || !next->tags[2])
			next = other;
		else
			next->addNext(other);
	}

	void undo();
	void redo();

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

	char getFrom() {
		return from;
	}

	char getTo() {
		return to;
	}

	Move *getNext() {
		return next;
	}

	Move *getLast() {
		return last;
	}

	
}