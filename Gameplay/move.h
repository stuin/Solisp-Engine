namespace Solisp {
	class Move;
}

#include <bitset>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/bitset.hpp>

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

#define MOVETAGCOUNT 4

enum move_tags { PLAYER, FLIP, VALID, LOOP };

static unsigned int max_id = 0;

class Solisp::Move {
private:
	friend class boost::serialization::access;
	template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & count & from & to & tags & id;
    }

	//Card movements
	int count;
	int from;
	int to;
	bitset<MOVETAGCOUNT> tags;

	//List references
	Move *next = NULL;
	Move *last;
	unsigned int id;

	//Check for circular card movement
	bool check_loop(Move *other) {
		if(this->from == other->from || this->to == other->to || other->from == 0)
			return false;

		if(this->from == other->to && this->to == other->from)
			return this->count == other->count;

		return check_loop(other->last);
	}

public:
	Move() { }

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
		this->id = ++max_id;
		//tags[LOOP] = check_loop(last);
	}

	//Recursizely delete backward
	~Move() {
		delete last;
	}

	//Recursizely delete forward
	void clear_forward() {
		if(next != NULL) {
			next->last = NULL;
			next->clear_forward();
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
			clear_forward();
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
	void correct_count(int count) {
		if(count < this->count)
			this->count = count;
	}

	//Get tag value
	bool get_tag(int tag) {
		if(tag >= 0 && tag < MOVETAGCOUNT)
			return tags[tag];
		return false;
	}

	//Normal getters
	int get_count() {
		return count;
	}

	int get_from() {
		return from;
	}

	int get_to() {
		return to;
	}

	unsigned int get_id() {
		return id;
	}

	Move *get_next() {
		return next;
	}

	Move *get_last() {
		return last;
	}
};
