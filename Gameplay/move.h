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

#define MOVETAGCOUNT 3
static unsigned int max_id = 0;

using unc = unsigned char;
enum move_tags { FLIP, VALID, LOOP };

class Solisp::Move {
private:
	friend class boost::serialization::access;
	template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & from & to & user & count & tags & id;
    }

	//Card movements
	unc from;
	unc to;
	unc user;
	unsigned int count;
	bitset<MOVETAGCOUNT> tags;

	//List references
	unsigned int id;
	Move *next = NULL;
	Move *last;

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
	Move(unc from, unc to, unsigned int count, unc user, bool flip, Move *last) {
		//Set general move
		this->count = count;
		this->from = from;
		this->to = to;
		this->user = user;

		//Set special tags
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
		if(user == 0 && from != 0 && last != NULL)
			last->undo();
	}

	//Revalidate next move
	void redo(bool first=true) {
		if(tags[VALID] && next != NULL)
			next->redo(true);
		else if(first || user == 0) {
			tags[VALID] = true;
			if(next != NULL)
				next->redo(false);
		}
	}

	//Set actual card count if move-all used
	void correct_count(unsigned int count) {
		if(count < this->count)
			this->count = count;
	}

	//Get tag value
	bool get_tag(int tag) {
		if(tag >= 0 && tag < MOVETAGCOUNT)
			return tags[tag];
		return false;
	}

	bool is_player() {
		return user != 0;
	}

	//Normal getters
	unc get_from() {
		return from;
	}

	unc get_to() {
		return to;
	}

	unsigned int get_count() {
		return count;
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
